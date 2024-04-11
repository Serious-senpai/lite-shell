#pragma once

#include <all.hpp>

class ExitCommand : public liteshell::BaseCommand
{
public:
    ExitCommand()
        : liteshell::BaseCommand(
              "exit",
              "Exit the shell with the specified exit code",
              "If no exit code is specified, the shell will exit with the current errorlevel.",
              {},
              liteshell::CommandConstraint(1, 2)) {}

    DWORD run(const liteshell::Context &context)
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

        return 0;
    }
};
