#include <client.hpp>
#include <standard.hpp>

#include "initialize.hpp"

// Windows uses UTF-16, but we process UTF-8 only
// https://utf8everywhere.org/

const char title[] = R"(Windows lightweight command shell
Repository: https://github.com/Serious-senpai/lite-shell
Contribute: https://github.com/Serious-senpai/lite-shell/pulls
Issue tracker: https://github.com/Serious-senpai/lite-shell/issues
Type "help" to get started.)";

int main(int argc, const char *argv[])
{
    Client client;
    initialize(&client);

    if (argc > 1)
    {
        auto command = utf_convert(std::wstring(GetCommandLineW())).substr(strlen(argv[0]));
        client.process_command(command);

        return client.get_errorlevel();
    }
    else
    {
        std::cout << title << std::endl;
        while (true)
        {
            std::cout << "\n"
                      << client.get_prompt();
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
}