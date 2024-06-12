#pragma once

#include <all.hpp>

class HelpCommand : public liteshell::BaseCommand
{
public:
    HelpCommand()
        : liteshell::BaseCommand(
              "help",
              "Get all commands or get help for a specific command",
              "Provides help information about shell commands.\n"
              "To get help for a specific command, specify its name as the first argument (e.g. \"help help\").",
              liteshell::CommandConstraint("command", "The command to get help for", false))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        try
        {
            auto name = context.get("command");
            auto wrapper = context.client->get_optional_command(name);
            if (wrapper.has_value())
            {
                std::cout << (*wrapper)->help();
            }
            else
            {
                auto error = liteshell::CommandNotFound(name, context.client->fuzzy_command_search(name).c_str());
                throw std::invalid_argument(error.message);
            }
        }
        catch (liteshell::ArgumentMissingError &)
        {
            auto commands = context.client->walk_commands();
            for (auto iter = commands.begin(); iter != commands.end();)
            {
                if ((*iter)->name[0] == '_') // hidden command
                {
                    iter = commands.erase(iter);
                }
                else
                {
                    iter++;
                }
            }

            std::size_t max_width = 0;
            for (auto &wrapper : commands)
            {
                max_width = std::max(max_width, 3 + wrapper->name.size());
            }

            for (auto &wrapper : commands)
            {
                std::cout << wrapper->name;
                for (auto i = wrapper->name.size(); i < max_width; i++)
                {
                    std::cout << " ";
                }
                std::cout << wrapper->description << std::endl;
            }
        }

        return 0;
    }
};
