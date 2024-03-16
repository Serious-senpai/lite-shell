#pragma once

#include "base.hpp"
#include "context.hpp"
#include "error.hpp"
#include "format.hpp"
#include "standard.hpp"
#include "subprocess.hpp"
#include "utils.hpp"
#include "wrapper.hpp"

BOOL WINAPI ctrl_handler(DWORD ctrl_type)
{
    return ctrl_type == CTRL_C_EVENT;
}

/*
Represents a command shell client. The application should hold only one instance of this class.

This class is responsible for managing commands, subprocesses and errorlevel. It also provides a method
to run the shell indefinitely.
*/
class Client
{
private:
    DWORD errorlevel = 0;

    std::set<ProcessInfoWrapper> subprocesses;

    std::vector<std::string> resolve_order;

    std::vector<CommandWrapper<BaseCommand>> wrappers;
    std::map<std::string, unsigned> commands;

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
        if (context.args.size() == 0)
        {
            throw std::invalid_argument("No command provided");
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
    const std::optional<CommandWrapper<BaseCommand>> get_command(const std::string &name) const
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

    std::string get_prompt()
    {
        return format("liteshell(%d)~%s>", errorlevel, get_working_directory().c_str());
    }

    void set_ignore_ctrl_c(bool ignore)
    {
        if (!SetConsoleCtrlHandler(ctrl_handler, ignore))
        {
            std::cerr << format_last_error("Warning: SetConsoleCtrlHandler ERROR") << std::endl;
        }
    }

    void run_forever()
    {
        set_ignore_ctrl_c(true);
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

    /*
    Process a command message.

    The process logic is as follows: If `message` points to an executable, it will be executed in a subprocess.
    Otherwise, the shell will attempt to find a command registered with `add_command()` and invoke it instead.

    @param message The command message to process.
    @return A pointer to the current client.
    */
    Client *process_command(const std::string &message)
    {
        auto stripped_message = strip(message);
        try
        {
            auto context = Context::get_context(this, stripped_message);

            // Find an executable first
            auto executable = resolve(context.args[0]);
            if (executable.has_value())
            {
                // Is an executable
                auto final_context = context.replace_call(*executable);

                auto subprocess = spawn_subprocess(final_context);
                if (final_context.is_background_request())
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

    /*
    An error handler that process exceptions thrown during command execution.

    @param e The exception object that was thrown.
    */
    void on_error(std::exception &e)
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
        ERROR_CODE(SubprocessCreationError, 903);

#undef ERROR_CODE
    }

    /*
    Find an executable that `token` points to.
    The function will first look in the current working directory, then in the directories specified in `resolve_order`.

    @param token The token to resolve.
    @return The path to the executable if found, `std::nullopt` otherwise.
    */
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

    /*
    Spawn a subprocess and execute `command` in it.

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
            throw SubprocessCreationError(format_last_error(format("Unable to create subprocess: %s", final_context.message.c_str())));
        }
    }

    /*
    Get the shell current errorlevel

    @return The current errorlevel
    */
    int get_errorlevel()
    {
        return errorlevel;
    }
};