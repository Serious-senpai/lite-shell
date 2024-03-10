#include <base.hpp>
#include <converter.hpp>
#include <invokers.hpp>
#include <split.hpp>
#include <standard.hpp>
#include <strip.hpp>

#include "commands/args.hpp"
#include "commands/exit.hpp"
#include "commands/type.hpp"

// Windows uses UTF-16, but we process UTF-8 only
// https://utf8everywhere.org/

std::string get_working_directory()
{
    wchar_t buffer[MAX_PATH];
    auto size = GetCurrentDirectoryW(MAX_PATH, buffer);

    if (size == 0)
    {
        throw std::runtime_error("Error getting current directory.");
    }

    return utf_convert(std::wstring(buffer, buffer + size));
}

const char title[] = R"(Windows lightweight command shell
Repository: https://github.com/Serious-senpai/lite-shell
Contribute: https://github.com/Serious-senpai/lite-shell/pulls
Issue tracker: https://github.com/Serious-senpai/lite-shell/issues)";

int main()
{
    std::vector<CommandInvoker<BaseCommand>> commands;
    commands.emplace_back(std::make_shared<ArgsCommand>());
    commands.emplace_back(std::make_shared<ExitCommand>());
    commands.emplace_back(std::make_shared<TypeCommand>());

    int errorlevel = 0;
    std::cout << title << std::endl;
    while (true)
    {
        std::cout << std::endl
                  << "liteshell(" << errorlevel << ")~" << get_working_directory() << ">";

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

            try
            {
                auto parsed = command.parse(arguments);
                errorlevel = command.run(parsed);
            }
            catch (std::exception &e)
            {
                errorlevel = 1000;

#define ERROR_CODE(exception_type, code)            \
    if (dynamic_cast<exception_type *>(&e) != NULL) \
    {                                               \
        errorlevel = code;                          \
    }
                ERROR_CODE(std::runtime_error, 900);
                ERROR_CODE(std::invalid_argument, 901);

                std::cerr << e.what() << std::endl;
            }
        }
    }

    return 0;
}