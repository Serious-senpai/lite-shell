#include "exit.hpp"

int main(int argc, const char **argv)
{
    auto command = ExitCommand();
    return command.run(argc, argv);
}
