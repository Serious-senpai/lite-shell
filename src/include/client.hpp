#pragma once

#include "base.hpp"
#include "context.hpp"
#include "error.hpp"
#include "format.hpp"
#include "standard.hpp"
#include "utils.hpp"
#include "wrapper.hpp"

class Client
{
private:
    const char BACKGROUND_SUFFIX = '%';

    int errorlevel = 0;
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
    std::vector<CommandWrapper<BaseCommand>> walk_commands() const
    {
        return wrappers;
    }

    std::optional<CommandWrapper<BaseCommand>> get_command(const std::string &name) const
    {
        auto iter = commands.find(name);
        if (iter == commands.end())
        {
            return std::nullopt;
        }
        return wrappers[iter->second];
    }

    std::string get_prompt()
    {
        return format("liteshell(%d)~%s>", errorlevel, get_working_directory().c_str());
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

    Client *process_command(const std::string &message, const std::optional<std::string> &command_name = std::nullopt)
    {
        auto stripped_message = strip(message);
        if (stripped_message.back() == BACKGROUND_SUFFIX)
        {
            spawn_subprocess(message.substr(0, stripped_message.size() - 1));
        }
        else
        {
            try
            {
                auto context = get_context(stripped_message);
                auto wrapper = command_name.has_value() ? get_command(*command_name) : get_command(context);
                errorlevel = wrapper.run(context);
            }
            catch (std::exception &e)
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

#undef ERROR_CODE
            }
        }

        return this;
    }

    void spawn_subprocess(const std::string &message)
    {
        // TODO: Implement this
    }

    int get_errorlevel()
    {
        return errorlevel;
    }
};