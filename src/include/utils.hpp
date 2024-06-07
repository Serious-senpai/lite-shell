#pragma once

#include "converter.hpp"
#include "format.hpp"
#include "join.hpp"

namespace utils
{
    /**
     * @brief Format the last error given by
     * [`GetLastError`](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror).
     *
     * @param message The message to prepend to the error
     * @return The formatted error message
     */
    std::string last_error(const std::string &message);

    /**
     * @brief Get the size of the console window using
     * [`GetConsoleScreenBufferInfo`](https://learn.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo).
     *
     * @return The number of columns and rows, respectively.
     */
    std::pair<SHORT, SHORT> get_console_size();

    /** @brief Get the current working directory */
    std::string get_working_directory();

    /**
     * @brief Get the path of the executable i.e. the path of shell.exe using
     * [`GetModuleFileNameW`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew).
     */
    std::string get_executable_path();

    /** @brief The CTRL handler used by the command shell */
    BOOL WINAPI ctrl_handler(DWORD ctrl_type);

    /** @brief Set whether to ignore Ctrl C signal */
    void set_ignore_ctrl_c(bool ignore);

    /** @brief List all files matching a specific pattern (typically used to list a directory) */
    std::vector<WIN32_FIND_DATAW> list_files(const std::string &__pattern);

    /**
     * @brief Remove a directory recursively. It is the caller's responsibility to ensure that the directory exists.
     *
     * @param directory The directory to remove
     * @param verbose Whether to display the deletion progress
     * @return Whether the directory was removed successfully
     */
    bool remove_directory(const std::string &directory, bool verbose);

    /**
     * @brief Whether a string has the specified prefix
     *
     * @param string The string to check
     * @param prefix The prefix to check for
     * @return Whether the string has the specified prefix
     */
    bool startswith(const std::string &string, const std::string &prefix);

    /**
     * @brief Whether a string has the specified suffix
     *
     * @param string The string to check
     * @param suffix The suffix to check for
     * @return Whether the string has the specified suffix
     */
    bool endswith(const std::string &string, const std::string &suffix);

    /**
     * @brief Escape a string to obtain a valid regex pattern
     *
     * @param original The original string
     * @return The escaped string
     */
    std::string regex_escape(const std::string &original);

    /**
     * @brief Whether a file is executable
     *
     * @param name The name of the file
     * @return Whether the file is executable
     */
    bool is_executable(LPCWSTR name);

    /**
     * @brief A helper method equivalent to `predicate ? first : second`
     */
    std::string ngettext(const bool predicate, const std::string &first, const std::string &second);

    /**
     * @brief Whether a character is a mathematical symbol
     *
     * @param c The character to check
     * @return Whether the character is a mathematical symbol
     */
    bool is_math_symbol(char c);

    const boost::regex _command_name = boost::regex(R"(^\w+$)");

    /**
     * @brief Whether a string is a valid command name
     *
     * @param name The name to check
     * @return Whether the name is a valid command name
     */
    bool is_valid_command(const std::string &name);

    const boost::regex _variable_name = boost::regex(R"(^\w+$)");

    /**
     * @brief Whether a string is a valid variable name
     *
     * @param name The name to check
     * @return Whether the name is a valid variable name
     */
    bool is_valid_variable(const std::string &name);

    const boost::regex _short_option_name = boost::regex(R"(^-[a-zA-Z]$)");

    /**
     * @brief Whether a string is a valid short option name
     *
     * @param name The name to check
     * @return Whether the name is a valid short option name
     */
    bool is_valid_short_option(const std::string &name);

    const boost::regex _long_option_name = boost::regex(R"(^--[a-zA-Z\-_]+$)");

    /**
     * @brief Whether a string is a valid long option name
     *
     * @param name The name to check
     * @return Whether the name is a valid long option name
     */
    bool is_valid_long_option(const std::string &name);

    /**
     * @brief Get the status of global memory using the native method
     * [`GlobalMemoryStatusEx`](https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex)
     *
     * @return The status of global memory
     */
    MEMORYSTATUSEX get_global_memory_status();

    /**
     * @brief Calculate the square root of a value using binary search
     *
     * @param value The value to calculate the square root of
     * @return The square root of the value
     */
    template <typename T>
    T sqrt(const T &value);

    /**
     * @brief Register a callback to run when this object is destroyed
     * @see https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization
     */
    class Finalize
    {
    private:
        const std::function<void()> callback;

        Finalize(const Finalize &) = delete;
        Finalize &operator=(const Finalize &) = delete;

    public:
        /** @brief Construct a new `Finalize` object with a registered callback */
        Finalize(const std::function<void()> &callback);

        ~Finalize();
    };
}
