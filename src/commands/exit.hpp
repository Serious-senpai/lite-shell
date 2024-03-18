#pragma once

#include <base.hpp>
#include <standard.hpp>

class ExitCommand : public BaseCommand
{
public:
    ExitCommand()
        : BaseCommand(
              "exit",
              "Exit the shell with the specified exit code",
              "If no exit code is specified, the shell will exit with the current errorlevel.",
              "exit <code: optional>",
              {}) {}

    DWORD run(const Context &context)
    {
        if (context.args.size() == 1)
        {
            exit(context.client->get_errorlevel());
        }
        else
        {
            auto code = std::stoi(context.args[1]);
            exit(code);
        }
    }
};
