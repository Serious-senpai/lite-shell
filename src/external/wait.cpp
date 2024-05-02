#include <all.hpp>

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        throw std::invalid_argument("Expect exactly 1 argument (the process ID)");
    }

    DWORD pid = std::stoul(argv[1]);
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (process == NULL)
    {
        throw std::runtime_error(utils::last_error(utils::format("Unable to get process %d", pid)));
    }

    WaitForSingleObject(process, INFINITE);
    return 0;
}