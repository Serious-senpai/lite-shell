#pragma once

#include <all.hpp>

class KillCommand : public liteshell::BaseCommand
{
public:
    KillCommand()
        : liteshell::BaseCommand(
              "kill",
              "Kill a subprocess with the given PID",
              "",
              {},
              liteshell::CommandConstraint(2, 3)) {}

    DWORD run(const liteshell::Context &context)
    {
        DWORD pid = std::stoul(context.args[1]);
        UINT exit_code = 1;
        if (context.args.size() == 3)
        {
            exit_code = std::stoul(context.args[2]);
        }

        auto iterators = context.client->get_subprocesses();
        for (auto wrapper = iterators.first; wrapper != iterators.second; wrapper++)
        {
            if (wrapper->pid() == pid)
            {
                wrapper->kill(exit_code);
                std::cout << "Terminated process " << pid << " with exit code " << exit_code << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
