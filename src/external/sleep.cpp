#include <all.hpp>

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        throw std::invalid_argument("Expect exactly 1 argument (duration in milliseconds)");
    }

    try
    {
        DWORD duration = std::stoul(argv[1]);
        Sleep(duration);
    }
    catch (std::out_of_range &)
    {
        throw std::invalid_argument("Value is too large to convert to DWORD");
    }

    return 0;
}
