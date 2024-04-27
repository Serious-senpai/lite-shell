#pragma once

namespace liteshell
{
    /**
     * @brief A wrapper of [PROCESS_INFORMATION](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information)
     * containing information of a subprocess
     */
    class ProcessInfoWrapper
    {
    private:
        bool suspended = false;

        /** @brief The underlying PROCESS_INFORMATION struct. The inner handles should never been closed. */
        const PROCESS_INFORMATION info;

    public:
        /** @brief The subprocess command line */
        const std::string command;

        /** @brief Construct a new `ProcessInfoWrapper` object */
        ProcessInfoWrapper(const PROCESS_INFORMATION &info, const std::string &command)
            : info(info), command(command) {}

        /** @brief Whether the subprocess is suspended */
        bool is_suspended() const
        {
            return suspended;
        }

        /** @brief Suspend the subprocess */
        void suspend()
        {
            if (!suspended)
            {
                if (SuspendThread(info.hThread) == (DWORD)-1)
                {
                    throw std::runtime_error(utils::last_error("SuspendThread ERROR"));
                }

                suspended = true;
            }
            else
            {
                throw std::runtime_error("The process has already been suspended");
            }
        }

        /** @brief Resume the subprocess */
        void resume()
        {
            if (suspended)
            {
                if (ResumeThread(info.hThread) == (DWORD)-1)
                {
                    throw std::runtime_error(utils::last_error("ResumeThread ERROR"));
                }

                suspended = false;
            }
            else
            {
                throw std::runtime_error("The process hasn't been suspended yet");
            }
        }

        /**
         * @brief Wait for the subprocess with timeout
         *
         * @param milliseconds The timeout in milliseconds, may equal to `INFINITE`
         */
        void wait(DWORD milliseconds)
        {
            WaitForSingleObject(info.hProcess, milliseconds);
        }

        /**
         * @brief Kill the subprocess
         *
         * @param exit_code The exit code of the subprocess
         */
        void kill(UINT exit_code)
        {
            if (!TerminateProcess(info.hProcess, exit_code))
            {
                throw std::runtime_error(utils::last_error("TerminateProcess ERROR"));
            }
        }

        /**
         * @brief Get the exit code of the subprocess
         *
         * @return The exit code of the subprocess
         */
        DWORD exit_code() const
        {
            DWORD exit_code;
            GetExitCodeProcess(info.hProcess, &exit_code);
            return exit_code;
        }

        /**
         * @brief Get the process ID of the subprocess
         *
         * @return The process ID of the subprocess
         */
        DWORD pid() const
        {
            return info.dwProcessId;
        }

        /**
         * @brief Get the thread ID of the subprocess
         *
         * @return The thread ID of the subprocess
         */
        DWORD tid() const
        {
            return info.dwThreadId;
        }
    };
}