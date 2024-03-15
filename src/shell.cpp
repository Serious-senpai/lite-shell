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

    std::cout << title << std::endl;
    client.run_forever();

    return 0;
}