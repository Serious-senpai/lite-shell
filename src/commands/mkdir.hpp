#pragma once

class MkdirCommand : public BaseCommand
{
public:
    MkdirCommand()
        : BaseCommand(
              "mkdir",
              "Make a new directory",
              "",
              {"md"},
              CommandConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        if (!CreateDirectoryW(utf_convert(context.args[1]).c_str(), NULL))
        {
            throw std::runtime_error(last_error("Unable to create directory"));
        }

        return 0;
    }
};
