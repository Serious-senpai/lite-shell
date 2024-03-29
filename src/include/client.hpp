#pragma once

#include "base.hpp"
#include "environment.hpp"
#include "stream.hpp"
#include "subprocess.hpp"
#include "wrapper.hpp"

#define BATCH_EXT ".firefly"
#define BUFFER_SIZE 4096

/*
Represents a command shell client. The application should hold only one instance of this class.

This class is responsible for managing commands, subprocesses and errorlevel. It also provides a method
to run the shell indefinitely.
*/
class Client
{
private:
    const std::vector<std::string> extensions = {".exe", BATCH_EXT};

    std::set<ProcessInfoWrapper> subprocesses;

    std::vector<CommandWrapper<BaseCommand>> wrappers;
    std::map<std::string, unsigned> commands;

    Environment *environment = nullptr;

    CommandWrapper<BaseCommand> get_command(const std::string &name)
    {
        auto iter = commands.find(name);
        if (iter == commands.end())
        {
            throw std::invalid_argument(format("Unrecognized command: %s", name.c_str()));
        }

        return wrappers[iter->second];
    }

    CommandWrapper<BaseCommand> get_command(const Context &context)
    {
        if (context.tokens.size() == 0)
        {
            throw std::invalid_argument("No command provided");
        }

        return get_command(context.tokens[0]);
    }

    std::string resolve_environment(const std::string &message)
    {
        std::string result, variable;
        bool is_variable = false;

        auto append_variable = [this, &result, &variable, &is_variable]()
        {
            result += this->environment->get_value(variable);
            variable.clear();
            is_variable = false;
        };

        for (unsigned i = 0; i < message.size(); i++)
        {
            if (message[i] == '$')
            {
                if (i > 0 && message[i - 1] == '$') // Escape character $$ -> $
                {
                    if (!variable.empty())
                    {
                        throw std::runtime_error(format("Unexpected error when resolving environment variable %s", variable.c_str()));
                    }

                    result += '$';
                    is_variable = false;
                }
                else
                {
                    if (!variable.empty())
                    {
                        append_variable();
                    }

                    is_variable = true;
                }
            }
            else if ((('a' <= message[i] && message[i] <= 'z') || ('A' <= message[i] && message[i] <= 'Z') || message[i] == '_'))
            {
                if (is_variable)
                {
                    variable += message[i];
                }
                else
                {
                    result += message[i];
                }
            }
            else
            {
                if (is_variable)
                {
                    append_variable();
                }

                result += message[i];
            }
        }

        if (!variable.empty())
        {
            append_variable();
        }

        return result;
    }

    /*
    @brief Find an executable that `token` points to.
    The function will first look in the current working directory, then in the directories specified in `resolve_order`.

    @param token The token to resolve. This token may contain path separators.
    @return The path to the executable if found, `std::nullopt` otherwise.
    */
    std::optional<std::string> resolve(const std::string &token)
    {
        auto t = strip(token, {'\\', '/'});
        auto find_executable = [this, &t](const std::string &directory) -> std::optional<std::string>
        {
            for (const auto &file : explore_directory(directory, t + "*"))
            {
                auto filename = utf_convert(std::wstring(file.cFileName));
                for (auto &extension : extensions)
                {
                    if (endswith(filename, extension))
                    {
                        return join(directory, t + extension);
                    }
                }
            }

            return std::nullopt;
        };

        auto result = find_executable(get_working_directory());
        if (result.has_value())
        {
            return *result;
        }

        for (const auto &directory : get_resolve_order())
        {
            result = find_executable(directory);
            if (result.has_value())
            {
                return *result;
            }
        }

        return std::nullopt;
    }

    Client *process_batch_file(const std::string &path)
    {
        // Warning: ifstream read in text mode may f*ck up in Windows: https://stackoverflow.com/a/8834004

        std::ifstream fstream(path, std::ios_base::in | std::ios_base::binary);
        std::string data;
        while (!fstream.eof())
        {
            char buffer[BUFFER_SIZE] = {};
            fstream.read(buffer, BUFFER_SIZE);
            data += buffer;
        }

        data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
        data += "\n:EOF\n";
        data += ECHO_ON;

        stream.write(data);

        fstream.close();
        return this;
    }

public:
    InputStream stream;

    Client() : environment(new Environment())
    {
        auto path = get_executable_path();
        auto size = path.size();
        while (size > 0 && path[size - 1] != '\\')
        {
            size--;
        }

        environment->set_variable("PATH", path.substr(0, size));
        environment->set_variable("errorlevel", "0");
    }

    Environment *get_environment() const
    {
        return environment;
    }

    ~Client()
    {
        delete environment;
    }

    /*
    Get all commands of the current command shell

    @return A vector containing all commands
    */
    std::set<CommandWrapper<BaseCommand>> walk_commands() const
    {
        return std::set<CommandWrapper<BaseCommand>>(wrappers.begin(), wrappers.end());
    }

    /*
    Get a command from the internal list of commands.
    This could also be used as a way to get aliases.

    @param name The name of the command to get.
    @return The command that was requested. If not found, returns an empty optional.
    */
    const std::optional<CommandWrapper<BaseCommand>> get_optional_command(const std::string &name) const
    {
        auto iter = commands.find(name);
        if (iter == commands.end())
        {
            return std::nullopt;
        }
        return wrappers[iter->second];
    }

    /*
    Get all subprocesses of the current shell.

    @return A set containing all subprocesses (both running and terminated processes).
    */
    const std::set<ProcessInfoWrapper> get_subprocesses() const
    {
        return subprocesses;
    }

    /*
    Adds a command into the internal list of commands.

    @param ptr A shared pointer to the command to add.
    @return A pointer to the current client.
    */
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

    void run_forever()
    {
        set_ignore_ctrl_c(true);
        while (true)
        {
            process_command(stream.getline(format("\nliteshell~%s>", get_working_directory().c_str()), 0));
        }
    }

    /*
    @brief Process a command message.

    The process logic is as follows: If `message` points to an executable, it will be executed in a subprocess.
    Otherwise, the shell will attempt to find a command registered with `add_command()` and invoke it instead.

    @param message The command message to process.
    @return A pointer to the current client.
    */
    Client *process_command(const std::string &message)
    {
        auto stripped_message = resolve_environment(strip(message));

        // Special sequences
        if (stripped_message == ECHO_ON)
        {
            stream.echo = true;
        }
        else if (stripped_message == ECHO_OFF)
        {
            stream.echo = false;
        }
        else if (stripped_message[0] == ':') // is a jump label
        {
            // pass
        }
        else
        {
            try
            {
                auto context = Context::get_context(this, stripped_message, CommandConstraint());

                // Find an executable first
                auto executable = resolve(context.tokens[0]);
                if (executable.has_value()) // Is an executable or batch file
                {
                    if (endswith(*executable, ".exe"))
                    {
                        auto final_context = context.replace_call(*executable);

                        auto subprocess = spawn_subprocess(final_context);
                        if (final_context.is_background_request())
                        {
                            environment->set_variable("errorlevel", "0");
                        }
                        else
                        {
                            WaitForSingleObject(subprocess.info.hProcess, INFINITE);

                            DWORD errorlevel;
                            GetExitCodeProcess(subprocess.info.hProcess, &errorlevel);
                            environment->set_variable("errorlevel", std::to_string(errorlevel));
                        }
                    }
                    else
                    {
                        process_batch_file(*executable);
                    }
                }
                else // Is a local command, throw if no match was found in get_command()
                {
                    auto wrapper = get_command(context);
                    auto constraint = wrapper.command->constraint;
                    auto errorlevel = wrapper.run(constraint.require_context_parsing ? context.parse(constraint) : context);
                    environment->set_variable("errorlevel", std::to_string(errorlevel));
                }
            }
            catch (std::exception &e)
            {
                on_error(e);
            }
        }

        return this;
    }

    /*
    @brief An error handler that process exceptions thrown during command execution.

    @param e The exception object that was thrown.
    */
    void on_error(std::exception &e)
    {
        DWORD errorlevel = 1000;

#define ERROR_CODE(exception_type, code)               \
    {                                                  \
        auto ptr = dynamic_cast<exception_type *>(&e); \
        if (ptr != NULL)                               \
        {                                              \
            errorlevel = code;                         \
            std::cerr << ptr->what() << std::endl;     \
        }                                              \
    }

        ERROR_CODE(std::runtime_error, LITE_SHELL_RUNTIME_ERROR);
        ERROR_CODE(std::invalid_argument, LITE_SHELL_INVALID_ARGUMENT);
        ERROR_CODE(std::bad_alloc, LITE_SHELL_BAD_ALLOC);
        ERROR_CODE(SubprocessCreationError, LITE_SHELL_SUBPROCESS_CREATION_ERROR);

#undef ERROR_CODE

        environment->set_variable("errorlevel", std::to_string(errorlevel));
    }

    std::vector<std::string> get_resolve_order() const
    {
        std::vector<std::string> result;
        std::string current;
        for (auto c : environment->get_value("PATH"))
        {
            if (c == ';')
            {
                result.emplace_back(current);
                current.clear();
            }
            else
            {
                current += c;
            }
        }

        if (!current.empty())
        {
            result.push_back(current);
        }

        return result;
    }

    /*
    @brief Spawn a subprocess and execute `command` in it.

    @param command The command to execute.
    @return A wrapper object containing information about the subprocess.
    */
    ProcessInfoWrapper spawn_subprocess(const Context &context)
    {
        STARTUPINFOW *startup_info = (STARTUPINFOW *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STARTUPINFOW));
        startup_info->cb = sizeof(startup_info);

        auto final_context = context.strip_background_request();

        PROCESS_INFORMATION process_info;
        auto success = CreateProcessW(
            NULL,                                      // lpApplicationName
            utf_convert(final_context.message).data(), // lpCommandLine
            NULL,                                      // lpProcessAttributes
            NULL,                                      // lpThreadAttributes
            true,                                      // bInheritHandles
            0,                                         // dwCreationFlags
            NULL,                                      // lpEnvironment
            NULL,                                      // lpCurrentDirectory
            startup_info,                              // lpStartupInfo
            &process_info                              // lpProcessInformation
        );
        HeapFree(GetProcessHeap(), 0, startup_info);

        ProcessInfoWrapper wrapper(final_context.message, process_info);
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
            throw SubprocessCreationError(last_error(format("Unable to create subprocess: %s", final_context.message.c_str())));
        }
    }

    /*
    @brief Get the current errorlevel of the shell

    @return The current errorlevel
    */
    DWORD get_errorlevel() const
    {
        return std::stoul(environment->get_value("errorlevel"));
    }
};