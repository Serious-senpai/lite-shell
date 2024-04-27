#pragma once

#include <all.hpp>

class PsCommand : public liteshell::BaseCommand
{
public:
    PsCommand()
        : liteshell::BaseCommand(
              "ps",
              "Get all subprocesses of the current shell, regardless of their states",
              "",
              {},
              liteshell::CommandConstraint()) {}

    DWORD run(const liteshell::Context &context)
    {
        utils::Table displayer("PID", "Command line", "Exit code", "Suspended");
        for (auto wrapper_ptr : context.client->get_subprocesses())
        {
            std::string status_display;
            auto status = wrapper_ptr->exit_code();
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

            std::string suspend_display = wrapper_ptr->is_suspended() ? "Yes" : "No";
            displayer.add_row(std::to_string(wrapper_ptr->pid()), wrapper_ptr->command, status_display, suspend_display);
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
