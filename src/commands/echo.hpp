#pragma once

#include <base.hpp>
#include <standard.hpp>

class EchoCommand : public BaseCommand
{
public:
    EchoCommand()
        : BaseCommand(
              "echo",
              "Display a message",
              "",
              {},
              ArgumentsConstraint()) {}

    DWORD run(const Context &context)
    {
        std::cout << context.get_arguments_string() << std::endl;
        return 0;
    }
};
