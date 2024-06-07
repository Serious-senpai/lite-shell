#pragma once

#include <all.hpp>

class CdCommand : public liteshell::BaseCommand
{
public:
    CdCommand()
        : liteshell::BaseCommand(
              "cd",
              "Get or set the working directory",
              "Display the name of or change the current directory.\n\n"
              "Call this command with no argument to get the working directory (similar to Unix shell's \"pwd\").\n"
              "When a positional argument is provided, the shell will attempt to change the working directory to\n"
              "the specified path. If the path is not found, an error will be returned.",
              liteshell::CommandConstraint("path", "The path to change the working directory to", false))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        try
        {
            auto target = context.get("path");
            if (!SetCurrentDirectoryW(utils::utf_convert(target).c_str()))
            {
                throw std::runtime_error(utils::last_error(utils::format("Error when changing directory to \"%s\"", target.c_str())));
            }
        }
        catch (liteshell::ArgumentMissingError &)
        {
            std::cout << utils::get_working_directory() << std::endl;
        }

        return 0;
    }
};
