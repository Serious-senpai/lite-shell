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
        Table displayer({"Name", "Value"});
        for (auto &[name, value] : context.client->get_environment()->get_values())
        {
            displayer.add_row({name, value});
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
