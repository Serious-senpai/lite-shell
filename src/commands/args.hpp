#pragma once

#include <base.hpp>
#include <standard.hpp>

class ArgsCommand : public BaseCommand
{
private:
public:
    using BaseCommand::run;

    ArgsCommand() : BaseCommand("args") {}

    bool accept_any_arguments() override
    {
        return true;
    }

    int run(const ParseResult &arguments) override
    {
        std::cout << "Positional arguments (" << arguments.positional_arguments.size() << "): ";
        if (!arguments.positional_arguments.empty())
        {
            for (unsigned i = 0; i < arguments.positional_arguments.size() - 1; i++)
            {
                std::cout << arguments.positional_arguments[i] << ", ";
            }
            std::cout << arguments.positional_arguments.back();
        }
        std::cout << std::endl;

        std::cout << "Named arguments:" << std::endl;
        for (const auto &group : arguments.alias_groups)
        {
            for (const auto &name : group)
            {
                std::cout << name << ": ";
                std::vector<std::string> values(arguments.get(name));
                if (!values.empty())
                {
                    for (unsigned i = 0; i < values.size() - 1; i++)
                    {
                        std::cout << values[i] << ", ";
                    }
                    std::cout << values.back();
                }
                std::cout << std::endl;
            }
        }

        return 0;
    }
};
