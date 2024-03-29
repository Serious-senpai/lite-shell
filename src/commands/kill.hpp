#pragma once

#include <all.hpp>

class KillCommand : public BaseCommand
{
public:
    KillCommand()
        : BaseCommand(
              "kill",
              "Kill a subprocess",
              "",
              {},
              CommandConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        DWORD pid = std::stoul(context.args[1]);
        for (auto &wrapper : context.client->get_subprocesses())
        {
            if (wrapper.info.dwProcessId == pid)
            {
                if (!TerminateProcess(wrapper.info.hProcess, 1))
                {
                    throw std::runtime_error(last_error("TerminateProcess ERROR"));
                }

                std::cout << "Terminated process " << pid << " with exit code 1" << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument(format("No process with PID %lu found", pid));
    }
};
