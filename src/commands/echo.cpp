#include "echo.hpp"

int main(int argc, const char **argv)
{
    auto command = EchoCommand();
    return command.run(argc, argv);
}
