#pragma once

#include <base.hpp>
#include <client.hpp>
#include <standard.hpp>
#include <tables.hpp>

class PsCommand : public BaseCommand
{
public:
    PsCommand()
        : BaseCommand(
              "ps",
              "Get all subprocesses of the current shell, regardless of their states",
              "ps",
              {}) {}

    int run(const Context &context)
    {
        Table displayer({"PID", "Command line", "Exit code"});
        for (auto &subprocess : context.client->get_subprocesses())
        {
            std::string status_display;
            DWORD status = 0;
            if (GetExitCodeProcess(subprocess.info.hProcess, &status))
            {
                if (status == STILL_ACTIVE)
                {
                    status_display = "STILL_ACTIVE";
                }
                else
                {
                    status_display = std::to_string(status);
                }
            }
            else
            {
                status_display = format_last_error("GetExitCodeProcess ERROR");
            }

            displayer.add_row({std::to_string(subprocess.info.dwProcessId), subprocess.message, status_display});
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
