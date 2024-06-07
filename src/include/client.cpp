#include "client.hpp"

namespace liteshell
{
    std::shared_ptr<Client> Client::_instance;

    CommandWrapper<BaseCommand> Client::_get_command(const std::string &name) const
    {
        auto iter = _commands.find(name);
        if (iter == _commands.end())
        {
            throw CommandNotFound(name, fuzzy_command_search(name).c_str());
        }

        return _wrappers[iter->second];
    }

    CommandWrapper<BaseCommand> Client::_get_command(const Context &context) const
    {
        if (context.tokens.empty())
        {
            throw std::invalid_argument("No command provided");
        }

        return _get_command(context.tokens[0]);
    }

    std::optional<std::string> Client::resolve(const std::string &token) const
    {
#ifdef DEBUG
        std::cout << "Resolving executable from \"" << token << "\"" << std::endl;
#endif

        auto find_executable = [this, &token](const std::string &directory) -> std::optional<std::string>
        {
            try
            {
                for (const auto &file : utils::list_files(utils::join(directory, token + "*")))
                {
                    auto filename = utils::utf_convert(std::wstring(file.cFileName));
                    for (auto &extension : _extensions)
                    {
                        if (utils::endswith(filename, extension))
                        {
                            return utils::join(directory, token + extension);
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

    void Client::process_batch_file(const std::string &path) const
    {
        // Warning: ifstream read in text mode may f*ck up in Windows: https://stackoverflow.com/a/8834004

        std::ifstream fstream(path, std::ios_base::in | std::ios_base::binary);
        std::string data;
        while (!fstream.eof())
        {
            char buffer[LITE_SHELL_BUFFER_SIZE];
            ZeroMemory(buffer, LITE_SHELL_BUFFER_SIZE);
            fstream.read(buffer, LITE_SHELL_BUFFER_SIZE);
            data += buffer;
        }

        data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
        data += "\n:EOF\n";
        data += ECHO_ON;

        _stream->write(data);

        fstream.close();
    }

    void Client::on_error(std::exception &e) const
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

    std::shared_ptr<Client> Client::get_instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::make_shared<Client>();
        }

        return _instance;
    }

    Client::Client() : _environment(std::make_unique<Environment>()), _stream(std::make_unique<InputStream>())
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

    Client::~Client()
    {
        for (auto &subprocess : _subprocesses)
        {
            delete subprocess;
        }
    }

    Environment *Client::get_environment() const
    {
        return _environment.get();
    }

    InputStream *Client::get_stream() const
    {
        return _stream.get();
    }

    std::set<CommandWrapper<BaseCommand>> Client::walk_commands() const
    {
        return std::set<CommandWrapper<BaseCommand>>(_wrappers.begin(), _wrappers.end());
    }

    std::optional<CommandWrapper<BaseCommand>> Client::get_optional_command(const std::string &name) const
    {
        auto iter = _commands.find(name);
        if (iter == _commands.end())
        {
            return std::nullopt;
        }
        return _wrappers[iter->second];
    }

    std::vector<ProcessInfoWrapper *> Client::get_subprocesses()
    {
        return _subprocesses;
    }

    Client *Client::add_command(const std::shared_ptr<BaseCommand> &ptr)
    {
        if (_commands.find(ptr->name) != _commands.end())
        {
            throw std::runtime_error(utils::format("Command %s already exists", ptr->name));
        }

        _wrappers.emplace_back(ptr);
        _commands[ptr->name] = _wrappers.size() - 1;
        for (auto &alias : ptr->aliases)
        {
            if (_commands.find(alias) != _commands.end())
            {
                throw std::runtime_error(utils::format("Command %s already exists", alias));
            }
            _commands[alias] = _wrappers.size() - 1;
        }

        return this;
    }

    template <typename T>
    Client *Client::add_command()
    {
        static_assert(std::is_base_of_v<BaseCommand, T>, "Can only add a subclass of BaseCommand as a command");
        return add_command(std::make_shared<T>());
    }

    std::string Client::fuzzy_command_search(const std::string &name) const
    {
        std::vector<std::string> all;
        for (auto &wrapper : _wrappers)
        {
            all.push_back(wrapper.command->name);
            for (auto &alias : wrapper.command->aliases)
            {
                all.push_back(alias);
            }
        }

        return *utils::fuzzy_search(all.begin(), all.end(), name);
    }

    void Client::run_forever()
    {
        utils::set_ignore_ctrl_c(true);
        while (true)
        {
            process_command(_stream->getline(utils::format("\nliteshell~%s>", utils::get_working_directory().c_str()), 0));
        }
    }

    void Client::process_command(const std::string &message)
    {
        _environment->set_value("cd", utils::get_working_directory().c_str());
        try
        {
            auto stripped_message = utils::strip(_environment->resolve(utils::strip(message)));
#ifdef DEBUG
            std::cout << utils::format("Processing command \"%s\"", stripped_message.c_str()) << std::endl;
#endif
            // Special sequences
            if (stripped_message == ECHO_ON)
            {
                _stream->echo = true;
            }
            else if (stripped_message == ECHO_OFF)
            {
                _stream->echo = false;
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
                auto context = Context::get_context(_instance, stripped_message);
                try
                {
                    auto wrapper = _get_command(context);

#ifdef DEBUG
                    std::cout << "Matched command \"" << wrapper.command->name << "\"" << std::endl;
#endif

                    auto constraint = wrapper.command->constraint;
                    auto errorlevel = wrapper.run(context.parse(constraint));
                    _environment->set_value("errorlevel", std::to_string(errorlevel));
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
        catch (std::exception &e)
        {
            on_error(e);
        }
    }

    std::vector<std::string> Client::get_resolve_order() const
    {
        return utils::split(_environment->get_value("PATH"), ';');
    }

    ProcessInfoWrapper *Client::spawn_subprocess(const Context &context)
    {
        auto final_context = context.strip_background_request();

        STARTUPINFOW startup_info;
        ZeroMemory(&startup_info, sizeof(startup_info));
        startup_info.cb = sizeof(startup_info);

        PROCESS_INFORMATION process_info;
        auto success = CreateProcessW(
            NULL,                                             // lpApplicationName
            utils::utf_convert(final_context.message).data(), // lpCommandLine
            NULL,                                             // lpProcessAttributes
            NULL,                                             // lpThreadAttributes
            TRUE,                                             // bInheritHandles
            0,                                                // dwCreationFlags
            NULL,                                             // lpEnvironment
            NULL,                                             // lpCurrentDirectory
            &startup_info,                                    // lpStartupInfo
            &process_info                                     // lpProcessInformation
        );

        if (success)
        {
            ProcessInfoWrapper *wrapper = new ProcessInfoWrapper(process_info, final_context.message);
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
            throw SubprocessCreationError(utils::last_error(utils::format("Unable to create subprocess \"%s\"", final_context.message.c_str())));
        }
    }

    DWORD Client::get_errorlevel() const
    {
        return std::stoul(_environment->get_value("errorlevel"));
    }
}
