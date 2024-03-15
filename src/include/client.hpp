#pragma once

#include "base.hpp"
#include "context.hpp"
#include "error.hpp"
#include "format.hpp"
#include "standard.hpp"
#include "subprocess.hpp"
#include "utils.hpp"
#include "wrapper.hpp"

class Client
{
private:
    const char BACKGROUND_SUFFIX = '%';

    DWORD errorlevel = 0;

    std::set<ProcessInfoWrapper> subprocesses;

    std::vector<std::string> resolve_order;

    std::vector<CommandWrapper<BaseCommand>> wrappers;
    std::map<std::string, unsigned> commands;

    Context get_context(const std::string &message)
    {
        auto tokens = split(message);

        std::vector<std::string> args;
        std::map<std::string, std::vector<std::string>> kwargs;

        std::optional<std::string> current_parameter;
        auto add_token = [this, &args, &kwargs, &current_parameter](const std::string &token)
        {
            if (current_parameter.has_value())
            {
                // keyword argument
                kwargs[*current_parameter].push_back(token);
            }
            else
            {
                // positional argument
                args.push_back(token);
            }
        };

        for (auto &token : tokens)
        {
            if (token[0] == '-')
            {
                if (token.size() == 1) // Token "-"
                {
                    throw CommandInputError("Input pipe is not supported");
                }
                else if (token[1] != '-') // Token of type "-abc", treat it as "-a", "-b", "-c"
                {
                    for (unsigned i = 1; i < token.size(); i++)
                    {
                        std::string name = "-";
                        name += token[i];

                        if (token[i] < 'a' || token[i] > 'z')
                        {
                            throw CommandInputError(format("Unsupported option: %s", name.c_str()));
                        }

                        current_parameter = name;
                        add_token(name);
                    }
                }
                else // Token of type "--abc"
                {
                    current_parameter = token;
                    add_token(token);
                }
            }
            else
            {
                add_token(token);
            }
        }

        return Context(message, args, kwargs, this);
    }

    CommandWrapper<BaseCommand> get_command(const std::string &name)
    {
        auto iter = commands.find(name);
        if (iter == commands.end())
        {
            throw CommandInputError(format("Unrecognized command: %s", name.c_str()));
        }

        return wrappers[iter->second];
    }

    CommandWrapper<BaseCommand> get_command(const Context &context)
    {
        if (context.args.size() == 0)
        {
            throw CommandInputError("No command provided");
        }

        return get_command(context.args[0]);
    }

public:
    Client()
    {
        auto path = get_executable_path();
        auto size = path.size();
        while (size > 0 && path[size - 1] != '\\')
        {
            size--;
        }

        resolve_order.push_back(path.substr(0, size));
    }

    const std::vector<CommandWrapper<BaseCommand>> walk_commands() const
    {
        return wrappers;
    }

    const std::optional<CommandWrapper<BaseCommand>> get_command(const std::string &name) const
    {
        auto iter = commands.find(name);
        if (iter == commands.end())
        {
            return std::nullopt;
        }
        return wrappers[iter->second];
    }

    const std::set<ProcessInfoWrapper> get_subprocesses() const
    {
        return subprocesses;
    }

    bool is_background_request(const std::string &message)
    {
        auto stripped_message = strip(message);
        auto n = stripped_message.size();
        return n > 2 && stripped_message[n - 2] == ' ' && stripped_message[n - 1] == '%';
    }

    std::string get_prompt()
    {
        return format("liteshell(%d)~%s>", errorlevel, get_working_directory().c_str());
    }

    void run_forever()
    {
        if (!SetConsoleCtrlHandler(NULL, TRUE))
        {
            std::cerr << format_last_error("Warning: SetConsoleCtrlHandler ERROR") << std::endl;
        }

        while (true)
        {
            std::cout << "\n"
                      << get_prompt();
            std::cout.flush();

            std::string input;
            std::getline(std::cin, input);

            if (std::cin.fail() || std::cin.eof())
            {
                std::cin.clear();
                continue;
            }

            if (input.empty())
            {
                continue;
            }

            process_command(input);
        }
    }

    Client *add_command(const std::shared_ptr<BaseCommand> &ptr)
    {
        if (commands.find(ptr->name) != commands.end())
        {
            throw std::runtime_error(format("Command %s already exists", ptr->name));
        }

        wrappers.emplace_back(ptr);
        commands[ptr->name] = wrappers.size() - 1;
        for (auto &alias : ptr->aliases)
        {
            if (commands.find(alias) != commands.end())
            {
                throw std::runtime_error(format("Command %s already exists", alias));
            }
            commands[alias] = wrappers.size() - 1;
        }

        return this;
    }

    Client *process_command(const std::string &message)
    {
        auto stripped_message = strip(message);
        try
        {
            auto context = get_context(stripped_message);

            // Find an executable first
            auto executable = resolve(context.args[0]);
            if (executable.has_value())
            {
                // Is an executable
                auto final_context = context.replace_call(*executable);
                auto subprocess = spawn_subprocess(final_context.message);
                if (is_background_request(message))
                {
                    errorlevel = 0;
                }
                else
                {
                    WaitForSingleObject(subprocess.info.hProcess, INFINITE);
                    GetExitCodeProcess(subprocess.info.hProcess, &errorlevel);
                }
            }
            else
            {
                // Is a local command, throw if no match was found in get_command()
                auto wrapper = get_command(context);
                errorlevel = wrapper.run(context);
            }
        }
        catch (std::exception &e)
        {
            on_error(e);
        }

        return this;
    }

    void on_error(std::exception e)
    {
        errorlevel = 1000;

#define ERROR_CODE(exception_type, code)               \
    {                                                  \
        auto ptr = dynamic_cast<exception_type *>(&e); \
        if (ptr != NULL)                               \
        {                                              \
            errorlevel = code;                         \
            std::cerr << ptr->what() << std::endl;     \
        }                                              \
    }

        ERROR_CODE(std::runtime_error, 900);
        ERROR_CODE(std::invalid_argument, 901);
        ERROR_CODE(std::bad_alloc, 902);
        ERROR_CODE(CommandInputError, 903);
        ERROR_CODE(SubprocessCreationError, 904);

#undef ERROR_CODE
    }

    std::optional<std::string> resolve(const std::string &token)
    {
        auto find_executable = [&token](const std::string &directory) -> std::optional<std::string>
        {
            auto name = join(directory, token);
            if (is_executable(utf_convert(name).c_str()))
            {
                return name;
            }

            name += ".exe";
            if (is_executable(utf_convert(name).c_str()))
            {
                return name;
            }

            return std::nullopt;
        };

        auto result = find_executable(get_working_directory());
        if (result.has_value())
        {
            return *result;
        }

        for (const auto &directory : resolve_order)
        {
            result = find_executable(directory);
            if (result.has_value())
            {
                return *result;
            }
        }

        return std::nullopt;
    }

    ProcessInfoWrapper spawn_subprocess(const std::string &command)
    {
        STARTUPINFOW *startup_info = (STARTUPINFOW *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STARTUPINFOW));
        startup_info->cb = sizeof(startup_info);

        // Strip background request suffix
        std::string cmd(strip(command));
        while (is_background_request(cmd))
        {
            cmd = strip(cmd.substr(0, cmd.size() - 1));
        }

        PROCESS_INFORMATION process_info;
        auto success = CreateProcessW(
            NULL,                    // lpApplicationName
            utf_convert(cmd).data(), // lpCommandLine
            NULL,                    // lpProcessAttributes
            NULL,                    // lpThreadAttributes
            true,                    // bInheritHandles
            0,                       // dwCreationFlags
            NULL,                    // lpEnvironment
            NULL,                    // lpCurrentDirectory
            startup_info,            // lpStartupInfo
            &process_info            // lpProcessInformation
        );
        HeapFree(GetProcessHeap(), 0, startup_info);

        ProcessInfoWrapper wrapper(command, process_info);
        if (success)
        {
            subprocesses.insert(wrapper);

            // Do not close the handles since we want to keep track of all subprocesses in our current shell (unless explicitly closed)

            /*
            SECURITY_ATTRIBUTES sec_attrs;
            sec_attrs.nLength = sizeof(SECURITY_ATTRIBUTES);
            sec_attrs.bInheritHandle = true;
            sec_attrs.lpSecurityDescriptor = NULL;

            // Open the thread and close the handle immediately
            // https://stackoverflow.com/a/11226526
            CloseHandle(CreateThread(&sec_attrs, 0, waiter_thread, &process_info, 0, NULL));
            */

            return wrapper;
        }
        else
        {
            wrapper.close();
            throw SubprocessCreationError(format_last_error(format("Unable to create subprocess: %s", command.c_str())));
        }
    }

    int get_errorlevel()
    {
        return errorlevel;
    }
};