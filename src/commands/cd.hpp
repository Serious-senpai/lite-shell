#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <error.hpp>
#include <standard.hpp>

class CdCommand : public BaseCommand
{
public:
    CdCommand()
        : BaseCommand(
              "cd",
              "Change the working directory",
              "cd <target>",
              {}) {}

    DWORD run(const Context &context)
    {
        if (context.args.size() == 1)
        {
            throw std::invalid_argument("No target directory specified");
        }
        else if (context.args.size() > 2)
        {
            throw std::invalid_argument("Expected 1 argument only");
        }
        else
        {
            auto target = context.args[1];
            if (!SetCurrentDirectoryW(utf_convert(target).c_str()))
            {
                throw std::runtime_error(format_last_error("Error when changing directory"));
            }
        }

        return 0;
    }
};
