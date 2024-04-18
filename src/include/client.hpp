#pragma once

#include "base.hpp"
#include "environment.hpp"
#include "fuzzy_search.hpp"
#include "stream.hpp"
#include "subprocess.hpp"
#include "wrapper.hpp"

#define BATCH_EXT ".ff"
#define BUFFER_SIZE 4096

namespace liteshell
{
    /**
     * Represents a command shell client. The application should hold only one instance of this class.
     *
     * TODO: Design this class as a singleton
     *
     * This class is responsible for managing commands, subprocesses and errorlevel. It also provides a method
     * to run the shell indefinitely.
     */
    class Client
    {
    private:
        const std::vector<std::string> extensions = {".exe", BATCH_EXT};

        std::vector<ProcessInfoWrapper> subprocesses;

        std::vector<CommandWrapper<BaseCommand>> wrappers;
        std::map<std::string, unsigned> commands;

        Environment *const environment;
        InputStream *const stream;

        CommandWrapper<BaseCommand> get_command(const std::string &name) const
        {
            auto iter = commands.find(name);
            if (iter == commands.end())
            {
                throw CommandNotFound(name, fuzzy_command_search(name).c_str());
            }

            return wrappers[iter->second];
        }

        CommandWrapper<BaseCommand> get_command(const Context &context) const
        {
            if (context.tokens.empty())
            {
                throw std::invalid_argument("No command provided");
            }

            return get_command(context.tokens[0]);
        }

        /**
         * @brief Find an executable that `token` points to.
         * The function will first look in the current working directory, then in the directories specified in `resolve_order`.
         *
         * @param token The token to resolve. This token may contain path separators.
         * @return The path to the executable if found, `std::nullopt` otherwise.
         */
        std::optional<std::string> resolve(const std::string &token) const
        {
#ifdef DEBUG
            std::cout << "Resolving executable from \"" << token << "\"" << std::endl;
#endif

            auto t = utils::strip(token, '\\', '/');
            auto find_executable = [this, &t](const std::string &directory) -> std::optional<std::string>
            {
                try
                {
                    for (const auto &file : utils::explore_directory(directory, t + "*"))
                    {
                        auto filename = utils::utf_convert(std::wstring(file.cFileName));
                        for (auto &extension : extensions)
                        {
                            if (utils::endswith(filename, extension))
                            {
                                return utils::join(directory, t + extension);
                            }
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    // pass
                }

                return std::nullopt;
            };

            auto result = find_executable(utils::get_working_directory());
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

        void process_batch_file(const std::string &path) const
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

            stream->write(data);

            fstream.close();
        }

    public:
        Client() : environment(new Environment()), stream(new InputStream())
        {
            auto path = utils::get_executable_path();
            auto size = path.size();
            while (size > 0 && path[size - 1] != '\\')
            {
                size--;
            }

            wchar_t buffer[32767];
            if (GetEnvironmentVariableW(L"PATH", buffer, 32767) == 0)
            {
                throw std::runtime_error(utils::last_error("Unable to get PATH"));
            }

            std::string env_path = utils::utf_convert(buffer);

            environment->set_value("PATH", path.substr(0, size) + ";" + env_path);
            environment->set_value("errorlevel", "0");
        }

        /**
         * @brief Get a pointer to the shell environment containing the variables
         *
         * @return A pointer to the shell environment
         */
        Environment *get_environment() const
        {
            return environment;
        }

        /**
         * @brief Get a pointer to the input stream
         *
         * @return A pointer to the input stream
         */
        InputStream *get_stream() const
        {
            return stream;
        }

        /**
         * Get all commands of the current command shell
         *
         * @return A set containing all commands
         */
        std::set<CommandWrapper<BaseCommand>> walk_commands() const
        {
            return std::set<CommandWrapper<BaseCommand>>(wrappers.begin(), wrappers.end());
        }

        /**
         * Get a command from the internal list of commands.
         * This could also be used as a way to get aliases.
         *
         * @param name The name of the command to get.
         * @return The command that was requested. If not found, returns an empty optional.
         */
        std::optional<CommandWrapper<BaseCommand>> get_optional_command(const std::string &name) const
        {
            auto iter = commands.find(name);
            if (iter == commands.end())
            {
                return std::nullopt;
            }
            return wrappers[iter->second];
        }

        /**
         * Get all subprocesses of the current shell.
         *
         * @return A pair of iterators to the beginning and end of the subprocesses set.
         */
        std::pair<std::vector<ProcessInfoWrapper>::iterator, std::vector<ProcessInfoWrapper>::iterator> get_subprocesses()
        {
            return std::make_pair(subprocesses.begin(), subprocesses.end());
        }

        /**
         * Adds a command into the internal list of commands.
         *
         * @param ptr A shared pointer `std::shared_ptr<BaseCommand>` to the command to add.
         * @return A pointer to the current client.
         */
        Client *add_command(const std::shared_ptr<BaseCommand> &ptr)
        {
            if (commands.find(ptr->name) != commands.end())
            {
                throw std::runtime_error(utils::format("Command %s already exists", ptr->name));
            }

            wrappers.emplace_back(ptr);
            commands[ptr->name] = wrappers.size() - 1;
            for (auto &alias : ptr->aliases)
            {
                if (commands.find(alias) != commands.end())
                {
                    throw std::runtime_error(utils::format("Command %s already exists", alias));
                }
                commands[alias] = wrappers.size() - 1;
            }

            return this;
        }

        std::string fuzzy_command_search(const std::string name) const
        {
            std::vector<std::string> all;
            for (auto &wrapper : wrappers)
            {
                all.push_back(wrapper.command->name);
                for (auto &alias : wrapper.command->aliases)
                {
                    all.push_back(alias);
                }
            }

            return *utils::fuzzy_search(all.begin(), all.end(), name);
        }

        /**
         * @brief Run the shell indefinitely.
         */
        void run_forever()
        {
            utils::set_ignore_ctrl_c(true);
            while (true)
            {
                process_command(stream->getline(utils::format("\nliteshell~%s>", utils::get_working_directory().c_str()), 0));
            }
        }

        /**
         * @brief Process a command message.
         *
         * @note The resolution order is as follows:
         * @note - Built-in command
         * @note - Executable/batch file
         *
         * @param message The command message to process.
         */
        void process_command(const std::string &message)
        {
            try
            {
                auto stripped_message = utils::strip(environment->resolve(utils::strip(message)));
#ifdef DEBUG
                std::cout << utils::format("Processing command \"%s\"", stripped_message.c_str()) << std::endl;
#endif
                // Special sequences
                if (stripped_message == ECHO_ON)
                {
                    stream->echo = true;
                }
                else if (stripped_message == ECHO_OFF)
                {
                    stream->echo = false;
                }
                else if (stripped_message.empty())
                {
                    // pass
                }
                else if (stripped_message[0] == ':') // is a jump label
                {
                    // pass
                }
                else
                {
                    auto context = Context::get_context(this, stripped_message);
                    try
                    {
                        auto wrapper = get_command(context);

#ifdef DEBUG
                        std::cout << "Matched command \"" << wrapper.command->name << "\"" << std::endl;
#endif

                        auto constraint = wrapper.command->constraint;
                        auto errorlevel = wrapper.run(context.parse(constraint));
                        environment->set_value("errorlevel", std::to_string(errorlevel));
                    }
                    catch (CommandNotFound &e)
                    {
#ifdef DEBUG
                        std::cout << "No command found. Resolving as an executable/script." << std::endl;
#endif

                        auto executable = resolve(context.tokens[0]);
                        if (executable.has_value()) // Is an executable or batch file
                        {
                            if (utils::endswith(*executable, ".exe"))
                            {
                                auto final_context = context.replace_call(*executable);

                                auto subprocess = spawn_subprocess(final_context);
                                if (final_context.is_background_request())
                                {
                                    environment->set_value("errorlevel", "0");
                                }
                                else
                                {
                                    subprocess.wait(INFINITE);
                                    environment->set_value("errorlevel", std::to_string(subprocess.exit_code()));
                                }
                            }
                            else
                            {
                                process_batch_file(*executable);
                            }
                        }
                        else
                        {
                            throw;
                        }
                    }
                }
            }
            catch (std::exception &e)
            {
                on_error(e);
            }
        }

        /**
         * @brief An error handler that process exceptions thrown during command execution.
         *
         * @param e The exception object that was thrown.
         */
        void on_error(std::exception &e) const
        {
            DWORD errorlevel = 1000;
            bool casted = false;

#define ERROR_CODE(exception_type, code)               \
    {                                                  \
        auto ptr = dynamic_cast<exception_type *>(&e); \
        if (ptr != NULL)                               \
        {                                              \
            errorlevel = code;                         \
            std::cerr << ptr->what() << std::endl;     \
            casted = true;                             \
        }                                              \
    }

            ERROR_CODE(std::runtime_error, 900);
            ERROR_CODE(std::invalid_argument, 901);
            ERROR_CODE(std::bad_alloc, 902);
            ERROR_CODE(SubprocessCreationError, 903);
            ERROR_CODE(EnvironmentResolveError, 904);
            ERROR_CODE(CommandNotFound, 905);
            ERROR_CODE(ArgumentMissingError, 906);
            ERROR_CODE(UnrecognizedOption, 907);
            ERROR_CODE(TooManyPositionalArguments, 908);

#undef ERROR_CODE

            if (!casted)
            {
                std::cerr << "An unknown exception occurred: " << e.what() << std::endl;
            }

            environment->set_value("errorlevel", std::to_string(errorlevel));
        }

        /**
         * @brief Split the PATH environment variable into a vector of paths.
         *
         * @return A vector of paths
         */
        std::vector<std::string> get_resolve_order() const
        {
            return utils::split(environment->get_value("PATH"), ';');
        }

        /**
         * @brief Spawn a subprocess and execute `command` in it.
         *
         * @param context A context holding the command to execute.
         * @return A wrapper object containing information about the subprocess.
         */
        ProcessInfoWrapper spawn_subprocess(const Context &context)
        {
            STARTUPINFOW *startup_info = (STARTUPINFOW *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STARTUPINFOW));
            startup_info->cb = sizeof(startup_info);

            auto final_context = context.strip_background_request();

            PROCESS_INFORMATION process_info;
            auto success = CreateProcessW(
                NULL,                                             // lpApplicationName
                utils::utf_convert(final_context.message).data(), // lpCommandLine
                NULL,                                             // lpProcessAttributes
                NULL,                                             // lpThreadAttributes
                true,                                             // bInheritHandles
                0,                                                // dwCreationFlags
                NULL,                                             // lpEnvironment
                NULL,                                             // lpCurrentDirectory
                startup_info,                                     // lpStartupInfo
                &process_info                                     // lpProcessInformation
            );
            HeapFree(GetProcessHeap(), 0, startup_info);

            ProcessInfoWrapper wrapper(process_info, final_context.message);
            if (success)
            {
                subprocesses.push_back(wrapper);

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
                CloseHandle(process_info.hProcess);
                CloseHandle(process_info.hThread);
                throw SubprocessCreationError(utils::last_error(utils::format("Unable to create subprocess: %s", final_context.message.c_str())));
            }
        }

        /**
         * @brief Get the current errorlevel of the shell
         *
         * @return The current errorlevel
         */
        DWORD get_errorlevel() const
        {
            return std::stoul(environment->get_value("errorlevel"));
        }
    };
}