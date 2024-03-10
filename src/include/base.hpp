#pragma once

#include "parse_results.hpp"
#include "standard.hpp"

class BaseCommand
{
private:
    // Mapping from aliases to their original names (original ones are mapped to themselves)
    std::map<std::string, std::string> arguments;

public:
    const std::string name;

    BaseCommand(const std::string &name) : name(name) {}
    virtual ~BaseCommand() {}

    // Whether this command accepts any arguments, including unregistered ones
    virtual bool accept_any_arguments()
    {
        return false;
    }

    virtual ParseResult parse(const std::vector<std::string> &args) final
    {
        std::vector<std::string> positional_arguments;
        std::vector<std::vector<std::string>> groups;
        std::map<std::string, unsigned> results;

        if (!accept_any_arguments())
        {
            for (const auto &[alias, original] : arguments)
            {
                if (results.find(original) == results.end())
                {
                    results[original] = groups.size();
                    groups.emplace_back();
                }

                if (alias != original)
                {
                    results[alias] = results[original];
                }
            }
        }

        std::optional<std::string> current_parameter;

        auto add_token = [this, &groups, &results, &positional_arguments, &current_parameter](const std::string &token)
        {
            if (current_parameter.has_value())
            {
                if (results.find(*current_parameter) == results.end())
                {
                    if (this->accept_any_arguments())
                    {
                        results[*current_parameter] = groups.size();
                        groups.emplace_back();
                    }
                    else
                    {
                        throw std::invalid_argument("Unknown argument: " + *current_parameter);
                    }
                }

                // keyword argument
                groups[results[*current_parameter]].push_back(token);
            }
            else
            {
                // positional argument
                positional_arguments.push_back(token);
            }
        };

        for (auto &arg : args)
        {
            if (arg[0] == '-')
            {
                if (arg.size() == 1) // Token "-"
                {
                    throw std::invalid_argument("Input pipe is not supported");
                }
                else if (arg[1] != '-') // Token of type "-abc", treat it as "-a", "-b", "-c"
                {
                    for (unsigned i = 1; i < arg.size(); i++)
                    {
                        std::string name = "-";
                        name += arg[i];

                        current_parameter = name;
                        add_token(name);
                    }
                }
                else // Token of type "--abc"
                {
                    current_parameter = arg;
                    add_token(arg);
                }
            }
            else
            {
                add_token(arg);
            }
        }

        return ParseResult(groups, results, positional_arguments, args);
    }

    virtual void add_argument(const std::initializer_list<std::string> &names) final
    {
        if (names.size() > 0)
        {
            for (auto iter = names.begin(); iter != names.end(); iter++)
            {
                arguments[*iter] = *names.begin();
            }
        }
    }

    virtual int run(const ParseResult &arguments) = 0;
    virtual int run(int argc, const char **argv) final
    {
        std::vector<std::string> args;
        for (int i = 0; i < argc; i++)
        {
            args.push_back(argv[i]);
        }

        return run(parse(args));
    }
};