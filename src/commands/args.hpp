#pragma once

#include <all.hpp>

class ArgsCommand : public liteshell::BaseCommand
{
private:
public:
    ArgsCommand()
        : BaseCommand(
              "args",
              "Describe the arguments that were passed in",
              "Display the arguments and keyword arguments passed to the command.\n"
              "This command was designed for testing the command line parser.",
              {},
              liteshell::CommandConstraint(false))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        std::vector<std::string> args;
        for (auto &name : context.args)
        {
            args.push_back("\"" + name + "\"");
        }
        std::cout << "Positional arguments (" << context.args.size() << "): " << utils::join(args.begin(), args.end(), ", ") << std::endl;

        std::vector<std::string> present;
        for (auto &name : context.present)
        {
            present.push_back("\"" + name + "\"");
        }

        std::cout << "Named arguments (" << present.size() << "): " << utils::join(present.begin(), present.end(), ", ") << std::endl;
        for (auto &[name, values] : context.kwargs)
        {
            std::vector<std::string> values_str;
            for (auto &value : values)
            {
                values_str.push_back("\"" + value + "\"");
            }

            std::cout << name << ": " << utils::join(values_str.begin(), values_str.end(), ", ") << std::endl;
        }

        return 0;
    }
};
