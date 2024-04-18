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
              liteshell::CommandConstraint("text", "The text to print to stdout", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        std::cout << context.get("text") << std::endl;
        return 0;
    }
};
