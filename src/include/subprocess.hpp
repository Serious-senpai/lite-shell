#pragma once

#include "utils.hpp"

namespace liteshell
{
    /**
     * @brief A wrapper of [`PROCESS_INFORMATION`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information)
     * containing information of a subprocess.
     */
    class ProcessInfoWrapper
    {
    private:
        bool _suspended = false;

        /** @brief The underlying PROCESS_INFORMATION struct. The inner handles should never be closed implicitly. */
        const PROCESS_INFORMATION _info;

        ProcessInfoWrapper(const ProcessInfoWrapper &) = delete;
        ProcessInfoWrapper &operator=(const ProcessInfoWrapper &) = delete;

    public:
        /** @brief The subprocess command line */
        const std::string command;

        /** @brief Construct a new `ProcessInfoWrapper` object */
        ProcessInfoWrapper(const PROCESS_INFORMATION &info, const std::string &command);

        /**
         * @brief Destructor for this object.
         *
         * Since `ProcessInfoWrapper` is non-copyable and non-movable, this destructor calls
         * [`CloseHandle`](https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle)
         * to close the underlying handles of the subprocess.
         */
        ~ProcessInfoWrapper();

        /** @brief Whether the subprocess is suspended */
        bool is_suspended() const;

        /** @brief Suspend the subprocess */
        void suspend();

        /** @brief Resume the subprocess */
        void resume();

        /**
         * @brief Wait for the subprocess with timeout
         *
         * @param milliseconds The timeout in milliseconds, may equal to `INFINITE`
         */
        void wait(DWORD milliseconds);

        /**
         * @brief Kill the subprocess
         *
         * @param exit_code The exit code of the subprocess
         */
        void kill(UINT exit_code);

        /**
         * @brief Get the exit code of the subprocess
         *
         * @return The exit code of the subprocess
         */
        DWORD exit_code() const;

        /**
         * @brief Get the process ID of the subprocess
         *
         * @return The process ID of the subprocess
         */
        DWORD pid() const;

        /**
         * @brief Get the thread ID of the subprocess
         *
         * @return The thread ID of the subprocess
         */
        DWORD tid() const;
    };
}