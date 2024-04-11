#pragma once

#include <all.hpp>

class EcholnCommand : public liteshell::BaseCommand
{
public:
    EcholnCommand()
        : liteshell::BaseCommand(
              "echoln",
              "Print to stdout and add a newline at the end",
              "",
              {},
              liteshell::CommandConstraint(2, 2)) {}

    DWORD run(const liteshell::Context &context)
    {
        std::cout << context.args[1] << std::endl;
        return 0;
    }
};
