#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <standard.hpp>
#include <utils.hpp>

class MkdirCommand : public BaseCommand
{
public:
    MkdirCommand()
        : BaseCommand(
              "mkdir",
              "Make a new directory",
              "",
              "mkdir <name: required>",
              {"md"},
              ArgumentsConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        if (!CreateDirectoryW(utf_convert(context.args[1]).c_str(), NULL))
        {
            throw std::runtime_error(format_last_error("Unable to create directory"));
        }

        return 0;
    }
};
