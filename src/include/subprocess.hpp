#pragma once

/* A wrapper of PROCESS_INFORMATION containing information of a subprocess */
class ProcessInfoWrapper
{
private:
    bool suspended = false;

    /* The underlying PROCESS_INFORMATION struct. The inner handles should never been closed. */
    const PROCESS_INFORMATION info;

public:
    /* The subprocess command line */
    const std::string command;

    ProcessInfoWrapper(const PROCESS_INFORMATION &info, const std::string &command)
        : info(info), command(command) {}

    /* Whether the subprocess is suspended */
    bool is_suspended() const
    {
        return suspended;
    }

    void suspend()
    {
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
        WaitForSingleObject(info.hProcess, milliseconds);
    }

    void kill(UINT exit_code)
    {
        if (!TerminateProcess(info.hProcess, exit_code))
        {
            throw std::runtime_error(last_error("TerminateProcess ERROR"));
        }
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
};
