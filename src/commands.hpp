#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "parse_results.hpp"

class BaseCommand
{
private:
    // Mapping from aliases to their original names (original ones are mapped to themselves)
    std::map<std::string, std::string> arguments;

public:
    const std::string name;

    BaseCommand(const std::string &name) : name(name) {}

    virtual ParseResult parse(const std::vector<std::string> &args) final
    {
        std::vector<std::string> positional_arguments;
        std::vector<std::vector<std::string>> groups;
        std::map<std::string, std::vector<std::vector<std::string>>::iterator> results;

        for (const auto &[alias, original] : arguments)
        {
            if (results.find(original) == results.end())
            {
                groups.emplace_back();
                results[original] = --groups.end();
            }

            if (alias != original)
            {
                results[alias] = results[original];
            }
        }

        std::optional<std::string> current_parameter;
        bool read_first = false;
        for (auto &arg : args)
        {
            if (read_first)
            {
                read_first = true;
                continue;
            }

            if (arg[0] == '-')
            {
                // arg is an argument name
                const auto iter = arguments.find(arg);
                if (iter == arguments.end())
                {
                    throw std::invalid_argument(format("Unknown argument %s", arg));
                }

                current_parameter = iter->second;
            }
            else
            {
                // arg is an argument value
                if (current_parameter.has_value())
                {
                    // keyword argument
                    results[*current_parameter]->push_back(arg);
                }
                else
                {
                    // positional argument
                    positional_arguments.push_back(arg);
                }
            }
        }

        return ParseResult(groups, results, positional_arguments);
    }

    virtual void add_command(const std::initializer_list<std::string> &names) final
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