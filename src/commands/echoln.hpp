#pragma once

class EcholnCommand : public BaseCommand
{
public:
    EcholnCommand()
        : BaseCommand(
              "echoln",
              "Print to stdout and add a newline at the end",
              "",
              {},
              CommandConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        std::cout << context.args[1] << std::endl;
        return 0;
    }
};
