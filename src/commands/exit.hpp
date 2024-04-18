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
              liteshell::CommandConstraint("exitcode", "The code to exit with", false)) {}

    DWORD run(const liteshell::Context &context)
    {
        try
        {
            auto code = std::stoul(context.get("exitcode"));
            exit(code);
        }
        catch (liteshell::ArgumentMissingError &e)
        {
            exit(context.client->get_errorlevel());
        }

        return 0;
    }
};
