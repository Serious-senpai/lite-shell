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
              liteshell::CommandConstraint("text", "The text to print to stdout", true, true)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto argument = context.values.at("text");
        std::cout << utils::join(argument.begin(), argument.end(), " ") << std::flush;
        return 0;
    }
};
