#pragma once

#include "base.hpp"
#include "context.hpp"
#include "error.hpp"
#include "format.hpp"
#include "split.hpp"
#include "standard.hpp"
#include "strip.hpp"
#include "wrapper.hpp"

class Client
{
private:
    int errorlevel = 0;
    std::vector<CommandWrapper<BaseCommand>> wrappers;
    std::map<std::string, unsigned> commands;

    std::pair<CommandWrapper<BaseCommand>, Context> get_context(const std::string &message)
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

        if (args.size() == 0)
        {
            throw CommandInputError("No command provided");
        }

        auto iter = commands.find(args[0]);
        if (iter == commands.end())
        {
            throw CommandInputError(format("Unrecognized command: %s", args[0].c_str()));
        }

        auto wrapper = wrappers[iter->second];
        return std::make_pair(wrapper, Context(message, args, kwargs, this));
    }

public:
    Client() {}

    std::vector<CommandWrapper<BaseCommand>> walk_commands()
    {
        return wrappers;
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
        try
        {
            auto [wrapper, context] = get_context(strip(message));
            errorlevel = wrapper.run(context);
        }
        catch (std::exception &e)
        {
            errorlevel = 1000;

#define ERROR_CODE(exception_type, code)            \
    if (dynamic_cast<exception_type *>(&e) != NULL) \
    {                                               \
        errorlevel = code;                          \
    }
            ERROR_CODE(std::runtime_error, 900);
            ERROR_CODE(std::invalid_argument, 901);
            ERROR_CODE(std::bad_alloc, 902);
            ERROR_CODE(CommandInputError, 903);

            std::cerr << e.what() << std::endl;
        }

        return this;
    }

    int get_errorlevel()
    {
        return errorlevel;
    }
};