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
              liteshell::CommandConstraint("text", "The text to print to stdout", true, true)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto argument = context.values.at("text");
        std::cout << utils::join(argument.begin(), argument.end(), " ") << std::endl;
        return 0;
    }
};
