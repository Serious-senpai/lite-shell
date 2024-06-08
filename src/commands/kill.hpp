#pragma once

#include <all.hpp>

class KillCommand : public liteshell::BaseCommand
{
public:
    KillCommand()
        : liteshell::BaseCommand(
              "kill",
              "Kill a subprocess with the given PID and exit code ",
              "",
              liteshell::CommandConstraint(
                  "pid", "The PID of the subprocess to kill", true,
                  "exit_code", "The exit code to use when killing the subprocess (default: 1)", false)) {}

    DWORD run(const liteshell::Context &context)
    {
        DWORD pid = std::stoul(context.get("pid"));
        UINT exit_code = 1;

        try
        {
            exit_code = std::stoul(context.get("exit_code"));
        }
        catch (liteshell::ArgumentMissingError &)
        {
            // pass
        }

        for (auto wrapper_ptr : context.client->get_subprocesses())
        {
            if (wrapper_ptr->pid() == pid)
            {
                wrapper_ptr->kill(exit_code);
                std::cout << "Terminated process " << pid << " with exit code " << exit_code << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
