#pragma once

#include "base.hpp"
#include "../standard.hpp"

class ExitCommand : public BaseCommand
{
public:
    ExitCommand() : BaseCommand("exit") {}

    int run(const ParseResult &arguments) override
    {
        if (arguments.positional_arguments.size() == 1)
        {
            exit(0);
        }
        else
        {
            auto code = std::stoi(arguments.positional_arguments[1]);
            exit(code);
        }
    }
};