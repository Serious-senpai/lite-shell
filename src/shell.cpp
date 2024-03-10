#include <base.hpp>
#include <converter.hpp>
#include <invokers.hpp>
#include <split.hpp>
#include <standard.hpp>
#include <strip.hpp>
#include <utils.hpp>

#include "commands/args.hpp"
#include "commands/cd.hpp"
#include "commands/echo.hpp"
#include "commands/exit.hpp"
#include "commands/ls.hpp"
#include "commands/type.hpp"

// Windows uses UTF-16, but we process UTF-8 only
// https://utf8everywhere.org/

const char title[] = R"(Windows lightweight command shell
Repository: https://github.com/Serious-senpai/lite-shell
Contribute: https://github.com/Serious-senpai/lite-shell/pulls
Issue tracker: https://github.com/Serious-senpai/lite-shell/issues)";

int main()
{
    std::vector<CommandInvoker<BaseCommand>> commands;
    commands.emplace_back(std::make_shared<ArgsCommand>());
    commands.emplace_back(std::make_shared<CdCommand>());
    commands.emplace_back(std::make_shared<EchoCommand>());
    commands.emplace_back(std::make_shared<ExitCommand>());
    commands.emplace_back(std::make_shared<LsCommand>());
    commands.emplace_back(std::make_shared<TypeCommand>());

    int errorlevel = 0;
    std::cout << title << std::endl;
    while (true)
    {
        std::cout << "\nliteshell(" << errorlevel << ")~" << get_working_directory() << ">";
        std::cout.flush();

        std::string input;
        std::getline(std::cin, input);

        if (input.empty())
        {
            continue;
        }

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
                ERROR_CODE(std::bad_alloc, 902);

                std::cerr << e.what() << std::endl;
            }
        }
    }

    return 0;
}