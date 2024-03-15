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
              "mkdir <name>",
              {"md"}) {}

    DWORD run(const Context &context)
    {
        if (context.args.size() != 2)
        {
            throw std::invalid_argument("Expect exactly 1 argument");
        }

        if (!CreateDirectoryW(utf_convert(context.args[1]).c_str(), NULL))
        {
            throw std::runtime_error(format_last_error("Unable to create directory"));
        }

        return 0;
    }
};
