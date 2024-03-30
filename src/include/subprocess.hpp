#pragma once

#include "standard.hpp"

/* A wrapper of PROCESS_INFORMATION containing information of a subprocess */
class ProcessInfoWrapper
{
private:
    bool closed = false;
    bool suspended = false;

    /* The underlying PROCESS_INFORMATION struct */
    const PROCESS_INFORMATION info;

    void check_valid() const
    {
        if (closed)
        {
            throw std::runtime_error("The process handle has already been closed");
        }
    }

public:
    /* The subprocess command line */
    const std::string command;

    ProcessInfoWrapper(const PROCESS_INFORMATION &info, const std::string &command)
        : info(info), command(command) {}

    /* Whether the handles of the underlying PROCESS_INFORMATION are closed */
    bool is_closed() const
    {
        return closed;
    }

    /* Whether the subprocess is suspended */
    bool is_suspended() const
    {
        return suspended;
    }

    void suspend()
    {
        check_valid();
        if (!suspended)
        {
            if (SuspendThread(info.hThread) == (DWORD)-1)
            {
                throw std::runtime_error(last_error("SuspendThread ERROR"));
            }

            suspended = true;
        }
        else
        {
            throw std::runtime_error("The process has already been suspended");
        }
    }

    void resume()
    {
        check_valid();
        if (suspended)
        {
            if (ResumeThread(info.hThread) == (DWORD)-1)
            {
                throw std::runtime_error(last_error("ResumeThread ERROR"));
            }

            suspended = false;
        }
        else
        {
            throw std::runtime_error("The process hasn't been suspended yet");
        }
    }

    void wait(DWORD milliseconds)
    {
        check_valid();
        WaitForSingleObject(info.hProcess, milliseconds);
    }

    void kill()
    {
        check_valid();
        if (!TerminateProcess(info.hProcess, 1))
        {
            throw std::runtime_error(last_error("TerminateProcess ERROR"));
        }

        close();
    }

    DWORD exit_code() const
    {
        DWORD exit_code;
        GetExitCodeProcess(info.hProcess, &exit_code);
        return exit_code;
    }

    DWORD pid() const
    {
        return info.dwProcessId;
    }

    DWORD tid() const
    {
        return info.dwThreadId;
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
