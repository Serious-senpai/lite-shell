#pragma once

#include <base.hpp>
#include <standard.hpp>

class ExitCommand : public BaseCommand
{
public:
    ExitCommand()
        : BaseCommand(
              "exit",
              "Exit the shell with the specified exit code (default: 0)",
              "",
              "exit <code: optional>",
              {}) {}

    DWORD run(const Context &arguments)
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
