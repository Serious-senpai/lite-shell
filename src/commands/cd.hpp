#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <error.hpp>
#include <standard.hpp>

class CdCommand : public BaseCommand
{
public:
    using BaseCommand::run;

    CdCommand() : BaseCommand("cd") {}

    int run(const ParseResult &arguments) override
    {
        if (arguments.positional_arguments.size() == 1)
        {
            throw std::invalid_argument("No target directory specified");
        }
        else if (arguments.positional_arguments.size() > 2)
        {
            throw std::invalid_argument("Expected 1 argument only");
        }
        else
        {
            auto target = arguments.positional_arguments[1];
            if (!SetCurrentDirectoryW(utf_convert(target).c_str()))
            {
                throw_last_error("Error when changing directory");
            }
        }

        return 0;
    }
};
