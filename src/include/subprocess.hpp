#pragma once

#include "standard.hpp"

class ProcessInfoWrapper
{
private:
    bool closed = false;

public:
    std::string command;
    PROCESS_INFORMATION info;

    ProcessInfoWrapper(const std::string &command, const PROCESS_INFORMATION &info)
        : command(command), info(info) {}

    bool is_closed()
    {
        return closed;
    }

    void close()
    {
        if (!closed)
        {
            CloseHandle(info.hProcess);
            CloseHandle(info.hThread);
            closed = true;
        }
    }
};

bool operator<(const ProcessInfoWrapper &first, const ProcessInfoWrapper &second)
{
    return first.info.dwProcessId < second.info.dwProcessId;
}

bool operator==(const ProcessInfoWrapper &first, const ProcessInfoWrapper &second)
{
    return first.info.dwProcessId == second.info.dwProcessId;
}