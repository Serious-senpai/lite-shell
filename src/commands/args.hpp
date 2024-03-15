#pragma once

#include <base.hpp>
#include <standard.hpp>

const char __description[] = R"(Display the arguments and keyword arguments passed to the command.

This command was designed for testing the command line parser only.
)";

class ArgsCommand : public BaseCommand
{
private:
public:
    ArgsCommand()
        : BaseCommand(
              "args",
              "Describe the arguments that were passed in",
              __description,
              "args <...>",
              {}) {}

    DWORD run(const Context &context)
    {
        std::cout << "Positional arguments (" << context.args.size() << "): ";
        if (!context.args.empty())
        {
            for (unsigned i = 0; i < context.args.size() - 1; i++)
            {
                std::cout << "\"" << context.args[i] << "\", ";
            }
            std::cout << "\"" << context.args.back() << "\"";
        }
        std::cout << std::endl;

        std::cout << "Named arguments:" << std::endl;
        for (const auto &[name, values] : context.kwargs)
        {
            std::cout << name << ": ";
            if (!values.empty())
            {
                for (unsigned i = 0; i < values.size() - 1; i++)
                {
                    std::cout << "\"" << values[i] << "\", ";
                }
                std::cout << "\"" << values.back() << "\"";
            }
            std::cout << std::endl;
        }

        return 0;
    }
};
