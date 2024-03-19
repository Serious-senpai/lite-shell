#pragma once

#include <base.hpp>
#include <client.hpp>
#include <standard.hpp>
#include <tables.hpp>

class PathCommand : public BaseCommand
{
public:
    PathCommand()
        : BaseCommand(
              "path",
              "View the current resolve paths",
              "",
              "path",
              {},
              ArgumentsConstraint(1, 1)) {}

    DWORD run(const Context &context)
    {
        Table displayer({"path"});
        for (auto &directory : context.client->resolve_order)
        {
            displayer.add_row({directory});
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
