#include "type.hpp"

int main(int argc, const char **argv)
{
    auto command = TypeCommand();
    return command.run(argc, argv);
}
