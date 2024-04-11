#pragma once

#include <all.hpp>

class SuspendCommand : public liteshell::BaseCommand
{
public:
    SuspendCommand()
        : liteshell::BaseCommand(
              "suspend",
              "Increase the suspend count of a subprocess with the given PID",
              "",
              {},
              liteshell::CommandConstraint(2, 2)) {}

    DWORD run(const liteshell::Context &context)
    {
        DWORD pid = std::stoul(context.args[1]);
        auto iterators = context.client->get_subprocesses();
        for (auto wrapper = iterators.first; wrapper != iterators.second; wrapper++)
        {
            if (wrapper->pid() == pid)
            {
                wrapper->suspend();
                std::cout << "Suspended process ID " << wrapper->pid() << ", thread ID " << wrapper->tid() << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
