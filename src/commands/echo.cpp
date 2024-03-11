#include <client.hpp>
#include <utils.hpp>

#include "echo.hpp"

int main(int argc, const char **argv)
{
    auto client = Client();
    return client.add_command(std::make_shared<EchoCommand>())
        ->process_command(join(argc, argv))
        ->get_errorlevel();
}
