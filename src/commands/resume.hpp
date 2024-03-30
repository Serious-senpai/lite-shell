#pragma once

class ResumeCommand : public BaseCommand
{
public:
    ResumeCommand()
        : BaseCommand(
              "resume",
              "Decrease the suspend count of a subprocess with the given PID",
              "",
              {},
              CommandConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        DWORD pid = std::stoul(context.args[1]);
        auto iterators = context.client->get_subprocesses();
        for (auto wrapper = iterators.first; wrapper != iterators.second; wrapper++)
        {
            if (wrapper->pid() == pid)
            {
                wrapper->resume();
                std::cout << "Resumed process ID " << wrapper->pid() << ", thread ID " << wrapper->tid() << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
