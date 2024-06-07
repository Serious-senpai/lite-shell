#include "subprocess.hpp"

namespace liteshell
{
    ProcessInfoWrapper::ProcessInfoWrapper(const PROCESS_INFORMATION &info, const std::string &command)
        : _info(info), command(command) {}

    ProcessInfoWrapper::~ProcessInfoWrapper()
    {
        CloseHandle(_info.hProcess);
        CloseHandle(_info.hThread);
    }

    bool ProcessInfoWrapper::is_suspended() const
    {
        return _suspended;
    }

    void ProcessInfoWrapper::suspend()
    {
        if (!_suspended)
        {
            if (SuspendThread(_info.hThread) == (DWORD)-1)
            {
                throw std::runtime_error(utils::last_error("SuspendThread ERROR"));
            }

            _suspended = true;
        }
        else
        {
            throw std::runtime_error("The process has already been suspended");
        }
    }

    void ProcessInfoWrapper::resume()
    {
        if (_suspended)
        {
            if (ResumeThread(_info.hThread) == (DWORD)-1)
            {
                throw std::runtime_error(utils::last_error("ResumeThread ERROR"));
            }

            _suspended = false;
        }
        else
        {
            throw std::runtime_error("The process hasn't been suspended yet");
        }
    }

    void ProcessInfoWrapper::wait(DWORD milliseconds)
    {
        WaitForSingleObject(_info.hProcess, milliseconds);
    }

    void ProcessInfoWrapper::kill(UINT exit_code)
    {
        if (!TerminateProcess(_info.hProcess, exit_code))
        {
            throw std::runtime_error(utils::last_error("TerminateProcess ERROR"));
        }
    }

    DWORD ProcessInfoWrapper::exit_code() const
    {
        DWORD exit_code;
        GetExitCodeProcess(_info.hProcess, &exit_code);
        return exit_code;
    }

    DWORD ProcessInfoWrapper::pid() const
    {
        return _info.dwProcessId;
    }

    DWORD ProcessInfoWrapper::tid() const
    {
        return _info.dwThreadId;
    }

}
