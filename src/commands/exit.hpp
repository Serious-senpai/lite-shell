#pragma once

#include <base.hpp>
#include <standard.hpp>

class ExitCommand : public BaseCommand
{
public:
    using BaseCommand::run;

    ExitCommand()
        : BaseCommand(
              "exit",
              "Exit the shell",
              "exit <optional code>",
              {}) {}

    int run(const Context &arguments)
    {
        if (arguments.args.size() == 1)
        {
            exit(0);
        }
        else
        {
            auto code = std::stoi(arguments.args[1]);
            exit(code);
        }
    }
};
