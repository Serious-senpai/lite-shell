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
              liteshell::CommandConstraint("text", "The text to print to stdout", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        std::cout << context.get("text");
        std::cout.flush();
        return 0;
    }
};
