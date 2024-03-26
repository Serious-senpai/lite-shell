#pragma once

#include <all.hpp>

class PsCommand : public BaseCommand
{
public:
    PsCommand()
        : BaseCommand(
              "ps",
              "Get all subprocesses of the current shell, regardless of their states",
              "",
              {},
              CommandConstraint(1, 1)) {}

    DWORD run(const Context &context)
    {
        Table displayer({"PID", "Command line", "Exit code"});
        for (auto &subprocess : context.client->get_subprocesses())
        {
            std::string status_display;
            DWORD status = 0;
            if (GetExitCodeProcess(subprocess.info.hProcess, &status))
            {
                switch (status)
                {
                case STILL_ACTIVE:
                    status_display = "STILL_ACTIVE";
                    break;
                case STATUS_CONTROL_C_EXIT:
                    status_display = "CONTROL_C_EXIT";
                    break;
                default:
                    status_display = std::to_string(status);
                    break;
                }
            }
            else
            {
                status_display = last_error("GetExitCodeProcess ERROR");
            }

            displayer.add_row({std::to_string(subprocess.info.dwProcessId), subprocess.command, status_display});
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
