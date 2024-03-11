#pragma once

#include <base.hpp>
#include <standard.hpp>

class SleepCommand : public BaseCommand
{
public:
    using BaseCommand::run;

    SleepCommand()
        : BaseCommand(
              "sleep",
              "Sleep for an amount of time (in milliseconds)",
              "sleep <time ms>",
              {}) {}

    int run(const Context &context)
    {
        if (context.args.size() != 2)
        {
            throw std::invalid_argument("Expected exactly 1 argument");
        }

        DWORD time = std::stoul(context.args[1]);
        Sleep(time);

        return 0;
    }
};
