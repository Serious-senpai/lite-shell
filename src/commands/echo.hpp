#pragma once

#include <base.hpp>
#include <standard.hpp>

class EchoCommand : public BaseCommand
{
public:
    using BaseCommand::run;

    EchoCommand() : BaseCommand("echo") {}

    bool accept_any_arguments() override
    {
        return true;
    }

    int run(const ParseResult &arguments) override
    {
        if (arguments.original.size() > 1) // Skip first argument
        {
            for (unsigned i = 1; i < arguments.original.size() - 1; i++)
            {
                std::cout << arguments.original[i] << " ";
            }
            std::cout << arguments.original.back();
        }

        std::cout << std::endl;
        return 0;
    }
};
