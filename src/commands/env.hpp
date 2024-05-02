#pragma once

#include <all.hpp>

class EnvCommand : public liteshell::BaseCommand
{
public:
    EnvCommand()
        : liteshell::BaseCommand(
              "env",
              "Display all environment variables",
              "",
              {},
              liteshell::CommandConstraint()) {}

    DWORD run(const liteshell::Context &context)
    {
        utils::Table displayer("Name", "Value");
        std::size_t columns = utils::get_console_size().first;
        displayer.limits = {30, columns - 30};

        for (auto &[name, value] : context.client->get_environment()->get_values())
        {
            displayer.add_row(name, value);
        }

        std::cout << displayer.display() << std::endl;
        return 0;
    }
};
