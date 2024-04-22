#include "initialize.hpp"

// Windows uses UTF-16, but we process UTF-8 only
// https://utf8everywhere.org/

const char title[] = R"(Windows lightweight command shell
Repository: https://github.com/Serious-senpai/lite-shell
Contribute: https://github.com/Serious-senpai/lite-shell/pulls
Issue tracker: https://github.com/Serious-senpai/lite-shell/issues
Type "help" to get started.
Type "help <command>" to get help about a command.
Type "<executable> <arguments> %" to run an executable in a subprocess.)";

int main(int argc, const char *argv[])
{
    auto client_ptr = liteshell::Client::get_instance();
    initialize(client_ptr.get());

    std::cout << title << std::endl;
    client_ptr->run_forever();

    return 0;
}
