#pragma once

#include <all.hpp>

const char __help_description[] = R"(Provides help information for Windows commands.

To get help for a specific command, specify its name as the first argument (e.g. "help help")
)";

class HelpCommand : public BaseCommand
{
public:
    HelpCommand()
        : BaseCommand(
              "help",
              "Get all commands or get help for a specific command",
              __help_description,
              {},
              CommandConstraint(1, 2)) {}

    DWORD run(const Context &context)
    {
        if (context.args.size() == 1) // Get all commands
        {
            for (auto &wrapper : context.client->walk_commands())
            {
                std::cout << wrapper.command->name << " - " << wrapper.command->description << std::endl;
            }
        }
        else // Get help for a specific command
        {
            auto name = context.args[1];
            auto wrapper = context.client->get_optional_command(name);
            if (wrapper.has_value())
            {
                std::cout << wrapper->command->help() << std::endl;
            }
            else
            {
                std::vector<std::string> commands;
                for (auto &wrapper : context.client->walk_commands())
                {
                    commands.push_back(wrapper.command->name);
                    for (auto &alias : wrapper.command->aliases)
                    {
                        commands.push_back(alias);
                    }
                }

                throw std::invalid_argument(
                    format(
                        "Command \"%s\" not found. Did you mean \"%s\"?",
                        name.c_str(),
                        fuzzy_search(commands.begin(), commands.end(), name)->c_str()));
            }
        }

        return 0;
    }
};
