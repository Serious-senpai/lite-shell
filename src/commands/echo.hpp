#pragma once

#include <all.hpp>

class EchoCommand : public BaseCommand
{
public:
    EchoCommand()
        : BaseCommand(
              "echo",
              "Print to stdout but do not add a newline like \"echoln\"",
              "",
              {},
              CommandConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        std::cout << context.args[1];
        std::cout.flush();
        return 0;
    }
};
