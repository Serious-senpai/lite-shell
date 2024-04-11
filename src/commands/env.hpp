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
              liteshell::CommandConstraint(1, 1)) {}

    DWORD run(const liteshell::Context &context)
    {
        for (auto &[name, value] : context.client->get_environment()->get_values())
        {
            std::cout << name << "=" << value << "\n";
        }

        return 0;
    }
};
