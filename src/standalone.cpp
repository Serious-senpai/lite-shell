#include <client.hpp>
#include <converter.hpp>

#include "initialize.hpp"

constexpr bool guarantee_fail()
{
    return false;
}

int main()
{
    Client client;
    initialize(&client);

    auto command = utf_convert(std::wstring(GetCommandLineW()));
#ifdef COMMAND_NAME
    client.process_command(command, COMMAND_NAME);
#else
#error "COMMAND_NAME must be defined"
#endif

    return client.get_errorlevel();
}