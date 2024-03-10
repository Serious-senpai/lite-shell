#define LITE_SHELL

#include <base.hpp>
#include <converter.hpp>
#include <invokers.hpp>
#include <split.hpp>
#include <standard.hpp>
#include <strip.hpp>

#include "commands/exit.hpp"

// Windows uses UTF-16, but we process UTF-8 only
// https://utf8everywhere.org/

std::string get_working_directory()
{
    TCHAR buffer[MAX_PATH];
    auto size = GetCurrentDirectory(MAX_PATH, buffer);

    if (size == 0)
    {
        throw std::runtime_error("Error getting current directory.");
    }

    return utf_convert(std::wstring(buffer, buffer + size));
}

int main()
{
    std::vector<CommandInvoker<BaseCommand>> commands;
    commands.emplace_back(std::make_shared<ExitCommand>());

    int errorlevel = 0;
    while (true)
    {
        std::cout << "furishell~" << get_working_directory() << ">";

        std::string input;
        std::getline(std::cin, input);

        auto arguments = split(strip(input));

        std::vector<CommandInvoker<BaseCommand>> matched;
        for (const auto &command : commands)
        {
            if (command.name == arguments[0])
            {
                matched.push_back(command);
            }
        }

        if (matched.empty())
        {
            std::cerr << "Command not found: " << arguments[0] << std::endl;
            errorlevel = 1;
        }
        else if (matched.size() > 1)
        {
            std::cerr << "Ambiguous command: " << arguments[0] << std::endl;
            errorlevel = 1;
        }
        else
        {
            auto command = matched[0];
            auto parsed = command.parse(arguments);
            errorlevel = command.run(parsed);
        }
    }

    return 0;
}