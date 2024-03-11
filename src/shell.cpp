#include <client.hpp>
#include <standard.hpp>

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
    Client client;
    client.add_command(std::make_shared<ArgsCommand>());
    client.add_command(std::make_shared<CdCommand>());
    client.add_command(std::make_shared<EchoCommand>());
    client.add_command(std::make_shared<ExitCommand>());
    client.add_command(std::make_shared<LsCommand>());
    client.add_command(std::make_shared<TypeCommand>());

    std::cout << title << std::endl;
    while (true)
    {
        std::cout << "\nliteshell(" << client.get_errorlevel() << ")~" << get_working_directory() << ">";
        std::cout.flush();

        std::string input;
        std::getline(std::cin, input);

        if (input.empty())
        {
            continue;
        }

        client.process_command(input);
    }

    return 0;
}