#pragma once

#include "standard.hpp"

/* A wrapper of PROCESS_INFORMATION containing information of a subprocess */
class ProcessInfoWrapper
{
private:
    bool closed = false;

public:
    /* The subprocess command line */
    std::string command;

    /* The underlying PROCESS_INFORMATION struct */
    PROCESS_INFORMATION info;

    ProcessInfoWrapper(const std::string &command, const PROCESS_INFORMATION &info)
        : command(command), info(info) {}

    /* Whether the handles of the underlying PROCESS_INFORMATION are closed */
    bool is_closed()
    {
        return closed;
    }

    /* Close the underlying handles of the subprocess (including a handle of its thread and itself) */
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