#pragma once

class KillCommand : public BaseCommand
{
public:
    KillCommand()
        : BaseCommand(
              "kill",
              "Kill a subprocess with the given PID",
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
                wrapper->kill();
                std::cout << "Terminated process " << pid << " with exit code 1" << std::endl;
                return 0;
            }
        }

        throw std::invalid_argument("Cannot find a subprocess with the given PID");
    }
};
