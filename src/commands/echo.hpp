#pragma once

#include <all.hpp>

class EchoCommand : public liteshell::BaseCommand
{
public:
    EchoCommand()
        : liteshell::BaseCommand(
              "echo",
              "Print to stdout but do not add a newline like \"echoln\"",
              "",
              {},
              liteshell::CommandConstraint(2, 2)) {}

    DWORD run(const liteshell::Context &context)
    {
        std::cout << context.args[1];
        std::cout.flush();
        return 0;
    }
};
