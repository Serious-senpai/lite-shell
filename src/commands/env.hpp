#pragma once

#include <all.hpp>

class EnvCommand : public BaseCommand
{
public:
    EnvCommand()
        : BaseCommand(
              "env",
              "Display all environment variables",
              "",
              {},
              CommandConstraint(1, 1)) {}

    DWORD run(const Context &context)
    {
        for (auto &[name, value] : context.client->get_environment()->get_values())
        {
            std::cout << name << "=" << value << "\n";
        }

        return 0;
    }
};
