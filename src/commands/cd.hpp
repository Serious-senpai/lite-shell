#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <error.hpp>
#include <standard.hpp>

const char __cd_description[] = R"(Display the name of or change the current directory.

Call this command with no argument to get the working directory (similar to Unix shell's "pwd").
When a positional argument is provided, the shell will attempt to change the working directory to
the specified path. If the path is not found, an error will be returned.
)";

class CdCommand : public BaseCommand
{
public:
    CdCommand()
        : BaseCommand(
              "cd",
              "Get or set the working directory",
              __cd_description,
              {},
              CommandConstraint(1, 2)) {}

    DWORD run(const Context &context)
    {
        if (context.args.size() == 1)
        {
            std::cout << get_working_directory() << std::endl;
        }
        else
        {
            auto target = context.args[1];
            if (!SetCurrentDirectoryW(utf_convert(target).c_str()))
            {
                throw std::runtime_error(format_last_error(format("Error when changing directory to %s", target.c_str())));
            }
        }

        return 0;
    }
};
