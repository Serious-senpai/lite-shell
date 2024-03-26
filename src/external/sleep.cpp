#include <all.hpp>

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Expect exactly 1 argument (duration in milliseconds)" << std::endl;
        return 1;
    }

    Sleep(std::stoull(argv[1]));
    return 0;
}