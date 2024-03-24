#pragma once

#include <base.hpp>
#include <standard.hpp>

class EcholnCommand : public BaseCommand
{
public:
    EcholnCommand()
        : BaseCommand(
              "echoln",
              "Print to stdout and add a newline at the end",
              "",
              {},
              ArgumentsConstraint()) {}

    DWORD run(const Context &context)
    {
        std::cout << context.get_arguments_string() << std::endl;
        return 0;
    }
};