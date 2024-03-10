#include "args.hpp"

int main(int argc, const char **argv)
{
    auto command = ArgsCommand();
    return command.run(argc, argv);
}
