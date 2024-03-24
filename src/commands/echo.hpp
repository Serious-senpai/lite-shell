#pragma once

#include <base.hpp>
#include <standard.hpp>

class EchoCommand : public BaseCommand
{
public:
    EchoCommand()
        : BaseCommand(
              "echo",
              "Print to stdout but do not add a newline like \"echoln\"",
              "",
              {},
              CommandConstraint()) {}

    DWORD run(const Context &context)
    {
        std::cout << context.get_arguments_string();
        std::cout.flush();
        return 0;
    }
};
