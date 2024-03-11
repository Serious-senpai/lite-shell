#include <client.hpp>
#include <utils.hpp>

#include "type.hpp"

int main(int argc, const char **argv)
{
    auto client = Client();
    return client.add_command(std::make_shared<TypeCommand>())
        ->process_command(join(argc, argv))
        ->get_errorlevel();
}
