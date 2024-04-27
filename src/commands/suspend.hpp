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
              liteshell::CommandConstraint("pid", "The PID of the target process", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        DWORD pid = std::stoul(context.get("pid"));
        for (auto wrapper_ptr : context.client->get_subprocesses())
        {
            if (wrapper_ptr->pid() == pid)
            {
                wrapper_ptr->suspend();
                std::cout << "Suspended process ID " << wrapper_ptr->pid() << ", thread ID " << wrapper_ptr->tid() << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
