#pragma once

#include "base.hpp"
#include "environment.hpp"
#include "finalize.hpp"
#include "fuzzy_search.hpp"
#include "maps.hpp"
#include "stream.hpp"
#include "style.hpp"
#include "subprocess.hpp"

#define LITE_SHELL_SCRIPT_EXTENSION ".ff"
#define LITE_SHELL_BUFFER_SIZE 4096

namespace liteshell
{
    /**
     * @brief The command shell application.
     *
     * Represents a command shell client. The application should hold only one instance of this class.
     *
     * This class is responsible for managing commands, subprocesses and errorlevel. It also provides a method
     * to run the shell indefinitely.
     */
    class Client
    {
    private:
        static std::shared_ptr<Client> _instance;
        static BOOL WINAPI _ctrl_handler(DWORD ctrl_type);

        std::vector<ProcessInfoWrapper *> _subprocesses;

        std::vector<std::shared_ptr<BaseCommand>> _wrappers;
        utils::CaseInsensitiveMap<std::size_t> _commands;

        const std::unique_ptr<Environment> _environment;
        const std::unique_ptr<InputStream> _stream;

        std::shared_ptr<BaseCommand> _get_command(const std::string &name) const
        {
            auto iter = _commands.find(name);
            if (iter == _commands.end())
            {
                throw CommandNotFound(name, fuzzy_command_search(name).c_str());
            }

            return _wrappers[iter->second];
        }

        std::shared_ptr<BaseCommand> _get_command(const Context &context) const
        {
            if (context.tokens.empty())
            {
                throw std::invalid_argument("No command provided");
            }

            return _get_command(context.tokens[0]);
        }

        /**
         * @brief Find an executable that `token` points to.
         *
         * The function will first look in the current working directory, then in the directories specified in `resolve_order`.
         *
         * @see https://stackoverflow.com/a/605139
         * @param token The token to resolve. This token may be a relative or absolute path.
         * @return The path to the executable if found, `std::nullopt` otherwise.
         */
        std::optional<std::string> resolve(const std::string &token) const
        {
#ifdef DEBUG
            std::cout << "Resolving executable from \"" << token << "\"" << std::endl;
#endif

            // `directory` may be empty
            std::function<std::optional<std::string>(const std::string &directory, const std::string &filepath)> search;
            search = [&search](const std::string &directory, const std::string &filepath) -> std::optional<std::string>
            {
                if (!utils::endswith(filepath, ".exe") && !utils::endswith(filepath, LITE_SHELL_SCRIPT_EXTENSION))
                {
                    auto result = search(directory, filepath + ".exe");
                    if (result.has_value())
                    {
                        return result;
                    }

                    return search(directory, filepath + LITE_SHELL_SCRIPT_EXTENSION);
                }

                try
                {
                    const auto fullpath = utils::join(directory, filepath);
#ifdef DEBUG
                    std::cout << "Searching " << fullpath << std::endl;
#endif
                    if (!utils::list_files(fullpath).empty())
                    {
                        return utils::get_absolute_path(fullpath);
                    }
                }
                catch (std::exception &)
                {
                    // pass
                }

                return std::nullopt;
            };

            // Search as an absolute path or a relative path to the working directory
            auto result = search("", token);
            if (result.has_value())
            {
                return result;
            }

            if (token.find('\\') == std::string::npos && token.find('/') == std::string::npos)
            {
                // token does not contain path separators
                for (const auto &directory : get_resolve_order())
                {
                    auto result = search(directory, token);
                    if (result.has_value())
                    {
                        return *result;
                    }
                }
            }

            return std::nullopt;
        }

        void process_batch_file(const std::string &path) const
        {
            // Warning: ifstream read in text mode may f*ck up in Windows: https://stackoverflow.com/a/8834004

#ifdef DEBUG
            std::cout << "Reading batch file: " << path << std::endl;
#endif

            auto file = CreateFileW(
                utils::utf_convert(path).c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            if (file == INVALID_HANDLE_VALUE)
            {
                throw std::runtime_error(utils::last_error("Error when opening file"));
            }

            auto _finalize = utils::Finalize(
                [&file]()
                {
                    CloseHandle(file);
                });

            char buffer[LITE_SHELL_BUFFER_SIZE];
            ZeroMemory(buffer, LITE_SHELL_BUFFER_SIZE);

            std::stringstream stream;

            DWORD read = LITE_SHELL_BUFFER_SIZE;
            while (read == LITE_SHELL_BUFFER_SIZE)
            {
                if (!ReadFile(file, buffer, LITE_SHELL_BUFFER_SIZE, &read, NULL))
                {
                    throw std::runtime_error(utils::last_error("Error when reading file"));
                }

                stream << std::string(buffer, buffer + read);
            }

            _stream->append_footer(stream);
            _stream->write(stream.str());
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

            _environment->set_value("errorlevel", std::to_string(errorlevel));
        }

        struct _command_compare
        {
            bool operator()(const std::shared_ptr<BaseCommand> &first, const std::shared_ptr<BaseCommand> &second) const
            {
                return first->name < second->name;
            }
        };

    public:
        /**
         * @brief Get the `Client` instance.
         *
         * @return A shared pointer to the `Client` instance
         */
        static std::shared_ptr<Client> get_instance();

        /**
         * @brief Construct a new `Client` object. Use `Client::get_instance()` instead.
         *
         * @see `Client::get_instance()`
         */
        Client() : _environment(std::make_unique<Environment>()), _stream(std::make_unique<InputStream>())
        {
            if (_instance != nullptr)
            {
                throw std::runtime_error("An instance of Client already exists");
            }

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

            _environment->set_value("PATH", path.substr(0, size) + ";" + env_path);
            _environment->set_value("errorlevel", "0");
        }

        /** @brief Destructor for this object */
        ~Client()
        {
            for (auto &subprocess : _subprocesses)
            {
                delete subprocess;
            }
        }

        /**
         * @brief Get a pointer to the shell environment containing the variables.
         *
         * @return A pointer to the shell environment
         */
        Environment *get_environment() const
        {
            return _environment.get();
        }

        /**
         * @brief Get a pointer to the input stream.
         *
         * @return A pointer to the input stream
         */
        InputStream *get_stream() const
        {
            return _stream.get();
        }

        /**
         * @brief Get all commands of the current command shell.
         *
         * @return A set containing all commands sorted according to their names.
         */
        std::set<std::shared_ptr<BaseCommand>, _command_compare> walk_commands() const
        {
            return std::set<std::shared_ptr<BaseCommand>, _command_compare>(_wrappers.begin(), _wrappers.end());
        }

        /**
         * @brief Get a command from the internal list of commands.
         * This can also be used as a way to get aliases.
         *
         * @param name The name of the command to get.
         * @return The command that was requested, or `std::nullopt` if not found.
         */
        std::optional<std::shared_ptr<BaseCommand>> get_optional_command(const std::string &name) const
        {
            auto iter = _commands.find(name);
            if (iter == _commands.end())
            {
                return std::nullopt;
            }
            return _wrappers[iter->second];
        }

        /**
         * @brief Get all subprocesses of the current shell.
         *
         * @return An array of pointers to `ProcessInfoWrapper` objects.
         */
        std::vector<ProcessInfoWrapper *> get_subprocesses()
        {
            return _subprocesses;
        }

        /**
         * @brief Add a command to the internal list of commands.
         *
         * @param ptr A shared pointer `std::shared_ptr<BaseCommand>` to the command to add.
         * @return A pointer to the current client to allow fluent-style chaining
         */
        Client *add_command(const std::shared_ptr<BaseCommand> &ptr)
        {
            if (_commands.find(ptr->name) != _commands.end())
            {
                throw std::runtime_error(utils::format("Command \"%s\" already exists", ptr->name.c_str()));
            }

            _wrappers.push_back(ptr);
            _commands[ptr->name] = _wrappers.size() - 1;
            for (auto &alias : ptr->aliases)
            {
                if (_commands.find(alias) != _commands.end())
                {
                    throw std::runtime_error(utils::format("Command \"%s\" already exists", alias.c_str()));
                }
                _commands[alias] = _wrappers.size() - 1;
            }

            return this;
        }

        /**
         * @brief Add a command to the internal list of commands.
         *
         * @tparam T A subclass of `BaseCommand`
         * @return A pointer to the current client to allow fluent-style chaining
         */
        template <typename T>
        Client *add_command()
        {
            static_assert(std::is_base_of_v<BaseCommand, T>, "Can only add a subclass of BaseCommand as a command");
            return add_command(std::make_shared<T>());
        }

        /**
         * @brief Search for a command that matches most closely to the given name.
         * @see `utils::fuzzy_search`
         *
         * @param name The name of the command to search for.
         * @return The name of the command that was found.
         */
        std::string fuzzy_command_search(const std::string &name) const
        {
            std::vector<std::string> all;
            for (auto &wrapper : _wrappers)
            {
                all.push_back(wrapper->name);
                for (auto &alias : wrapper->aliases)
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
            if (!SetConsoleCtrlHandler(_ctrl_handler, TRUE))
            {
                std::cerr << utils::last_error("Warning: SetConsoleCtrlHandler ERROR") << std::endl;
            }

            while (true)
            {
                process_command(
                    _stream->getline(
                        []()
                        {
                            SYSTEMTIME time;
                            GetLocalTime(&time);
                            std::cout << utils::format("\n[%d:%d:%d]", time.wHour, time.wMinute, time.wSecond);
                            utils::style_print("liteshell~", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                            std::cout << utils::get_working_directory() << ">";
                        },
                        0));
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
            _environment->set_value("cd", utils::get_working_directory().c_str());
            try
            {
                auto stripped_message = utils::strip(_environment->resolve(utils::strip(message)));
#ifdef DEBUG
                std::cout << utils::format("Processing command \"%s\"", stripped_message.c_str()) << std::endl;
#endif

                if (stripped_message.empty() || stripped_message[0] == ':')
                {
                    // pass
                }
                else
                {
                    auto context = Context::get_context(_instance, stripped_message, message);
                    try
                    {
                        auto wrapper = _get_command(context);

#ifdef DEBUG
                        std::cout << "Matched command \"" << wrapper->name << "\"" << std::endl;
#endif

                        auto constraint = wrapper->constraint;
                        auto errorlevel = wrapper->run(context.parse(constraint));
                        _environment->set_value("errorlevel", std::to_string(errorlevel));
                    }
                    catch (CommandNotFound &)
                    {
#ifdef DEBUG
                        std::cout << "No command found. Resolving as an executable/script." << std::endl;
#endif

                        auto executable = resolve(context.tokens[0]);

                        if (executable.has_value()) // Is an executable or batch file
                        {
#ifdef DEBUG
                            std::cout << "Matched executable/script " << *executable << std::endl;
#endif

                            if (utils::endswith(*executable, ".exe"))
                            {
                                auto final_context = context.replace_call(*executable);

                                auto subprocess = spawn_subprocess(
                                    final_context.strip_background_request().message,
                                    final_context.is_background_request(),
                                    false);
                                _environment->set_value("pid", std::to_string(subprocess->pid()));
                                if (final_context.is_background_request())
                                {
                                    _environment->set_value("errorlevel", "0");
                                }
                                else
                                {
                                    subprocess->wait(INFINITE);
                                    _environment->set_value("errorlevel", std::to_string(subprocess->exit_code()));
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
            catch (std::exception &error)
            {
                on_error(error);
            }
        }

        /**
         * @brief Split the PATH environment variable into a vector of paths.
         *
         * @return A vector of paths
         */
        std::vector<std::string> get_resolve_order() const
        {
            return utils::split(_environment->get_value("PATH"), ';');
        }

        /**
         * @brief Spawn a subprocess and execute `command` in it.
         *
         * @param command The command to execute in the subprocess.
         * @param background Whether to run the subprocess in the background.
         * @param new_console Whether to run the subprocess in a new console window. If this is `true`, parameter `background`
         * has no effect.
         * @return A pointer to the wrapper object containing information about the subprocess.
         */
        ProcessInfoWrapper *spawn_subprocess(const std::string &command, const bool background, const bool new_console)
        {
            DWORD flags = 0;
            if (background)
            {
                flags |= CREATE_NEW_PROCESS_GROUP;
            }
            if (new_console)
            {
                flags |= CREATE_NEW_CONSOLE;
            }

            STARTUPINFOW startup_info;
            ZeroMemory(&startup_info, sizeof(startup_info));
            startup_info.cb = sizeof(startup_info);

            PROCESS_INFORMATION process_info;
            auto success = CreateProcessW(
                NULL,                               // lpApplicationName
                utils::utf_convert(command).data(), // lpCommandLine
                NULL,                               // lpProcessAttributes
                NULL,                               // lpThreadAttributes
                TRUE,                               // bInheritHandles
                flags,                              // dwCreationFlags
                NULL,                               // lpEnvironment
                NULL,                               // lpCurrentDirectory
                &startup_info,                      // lpStartupInfo
                &process_info                       // lpProcessInformation
            );

            if (success)
            {
                ProcessInfoWrapper *wrapper = new ProcessInfoWrapper(process_info, command);
                _subprocesses.push_back(wrapper);

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
                throw SubprocessCreationError(utils::last_error(utils::format("Unable to create subprocess \"%s\"", command.c_str())));
            }
        }

        /**
         * @brief Get the current errorlevel of the shell.
         *
         * @return The current errorlevel
         */
        DWORD get_errorlevel() const
        {
            return std::stoul(_environment->get_value("errorlevel"));
        }
    };

    std::shared_ptr<Client> Client::_instance(nullptr);

    BOOL WINAPI Client::_ctrl_handler(DWORD ctrl_type)
    {
        if (ctrl_type == CTRL_C_EVENT)
        {
            auto client = Client::get_instance();
            client->get_stream()->clear();

            return TRUE;
        }
        return FALSE;
    }

    std::shared_ptr<Client> Client::get_instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::make_shared<Client>();
        }

        return _instance;
    }
}
