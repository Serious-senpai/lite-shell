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
        for (auto &wrapper : context.client->get_subprocesses())
        {
            if (wrapper.pid() == pid)
            {
                wrapper.suspend();
                std::cout << "Suspended process ID " << wrapper.pid() << ", thread ID " << wrapper.tid() << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
