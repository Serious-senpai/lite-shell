#pragma once

#include "converter.hpp"
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
    std::string last_error(const std::string &message)
    {
        return format("%s: %d", message.c_str(), GetLastError());
    }

    /**
     * @brief Get the size of the console window using
     * [`GetConsoleScreenBufferInfo`](https://learn.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo).
     *
     * @return The number of columns and rows, respectively.
     */
    std::pair<SHORT, SHORT> get_console_size()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        {
            throw std::runtime_error(last_error("GetConsoleScreenBufferInfo ERROR"));
        }

        SHORT columns = info.srWindow.Right - info.srWindow.Left + 1,
              rows = info.srWindow.Bottom - info.srWindow.Top + 1;

        return std::make_pair(columns, rows);
    }

    /** @brief Get the current working directory */
    std::string get_working_directory()
    {
        wchar_t buffer[MAX_PATH];
        auto size = GetCurrentDirectoryW(MAX_PATH, buffer);
        if (size == 0)
        {
            throw std::runtime_error(last_error("GetCurrentDirectoryW ERROR"));
        }

        return utf_convert(std::wstring(buffer, buffer + size));
    }

    /**
     * @brief Get the path of the executable i.e. the path of shell.exe using
     * [`GetModuleFileNameW`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew).
     */
    std::string get_executable_path()
    {
        wchar_t buffer[MAX_PATH];
        auto size = GetModuleFileNameW(NULL, buffer, MAX_PATH);
        if (size == 0)
        {
            throw std::runtime_error(last_error("Error calling GetModuleFileNameW"));
        }

        return utf_convert(std::wstring(buffer, buffer + size));
    }

    /** @brief The CTRL handler used by the command shell */
    BOOL WINAPI ctrl_handler(DWORD ctrl_type)
    {
        return ctrl_type == CTRL_C_EVENT;
    }

    /** @brief Set whether to ignore Ctrl C signal */
    void set_ignore_ctrl_c(bool ignore)
    {
        if (!SetConsoleCtrlHandler(ctrl_handler, ignore))
        {
            std::cerr << last_error("Warning: SetConsoleCtrlHandler ERROR") << std::endl;
        }
    }

    /** @brief Explore a directory */
    std::vector<WIN32_FIND_DATAW> explore_directory(const std::string &__directory, const std::string &__pattern = "\\*")
    {
        auto directory = join(__directory, __pattern);
        std::vector<WIN32_FIND_DATAW> results(1);

        HANDLE h_file = FindFirstFileW(utf_convert(directory).c_str(), &results[0]);
        if (h_file == INVALID_HANDLE_VALUE)
        {
            switch (GetLastError())
            {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_ACCESS_DENIED:
                return {};
            default:
                throw std::runtime_error(last_error("Error when listing directory"));
            }
        }

        do
        {
            results.emplace_back();
        } while (FindNextFileW(h_file, &results.back()));

        results.pop_back();

        if (!FindClose(h_file))
        {
            throw std::runtime_error(last_error("Error when closing file search handle"));
        }

        CloseHandle(h_file);
        return results;
    }

    /**
     * @brief Whether a string has the specified prefix
     *
     * @param string The string to check
     * @param prefix The prefix to check for
     * @return Whether the string has the specified prefix
     */
    bool startswith(const std::string &string, const std::string &prefix)
    {
        if (string.size() < prefix.size())
        {
            return false;
        }

        for (std::size_t i = 0; i < prefix.size(); i++)
        {
            if (string[i] != prefix[i])
            {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Whether a string has the specified suffix
     *
     * @param string The string to check
     * @param suffix The suffix to check for
     * @return Whether the string has the specified suffix
     */
    bool endswith(const std::string &string, const std::string &suffix)
    {
        if (string.size() < suffix.size())
        {
            return false;
        }

        for (std::size_t i = string.size() - suffix.size(); i < string.size(); i++)
        {
            if (string[i] != suffix[i + suffix.size() - string.size()])
            {
                return false;
            }
        }

        return true;
    }

    std::string regex_escape(const std::string &original)
    {
        std::stringstream result;
        for (char c : original)
        {
            if (c == '.' || c == '*' || c == '+' || c == '?' || c == '|' || c == '(' || c == ')' ||
                c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '^' || c == '$')
            {
                result << '\\';
            }
            result << c;
        }
        return result.str();
    }

    /**
     * @brief Whether a file is executable
     *
     * @param name The name of the file
     * @return Whether the file is executable
     */
    bool is_executable(LPCWSTR name)
    {
        DWORD _;
        return GetBinaryTypeW(name, &_);
    }

    /**
     * @brief A helper method equivalent to `predicate ? first : second`
     */
    std::string ngettext(const bool predicate, const std::string &first, const std::string &second)
    {
        return predicate ? first : second;
    }

    /**
     * @brief Whether a character is a mathematical symbol
     *
     * @param c The character to check
     * @return Whether the character is a mathematical symbol
     */
    bool is_math_symbol(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == ' ' || c == '%' || ('0' <= c && c <= '9') || c == '(' || c == ')';
    }

    const boost::regex _command_name = boost::regex(R"(^\w+$)");

    /**
     * @brief Whether a string is a valid command name
     *
     * @param name The name to check
     * @return Whether the name is a valid command name
     */
    bool is_valid_command(const std::string &name)
    {
        return boost::regex_match(name, _command_name);
    }

    const boost::regex _variable_name = boost::regex(R"(^\w+$)");

    /**
     * @brief Whether a string is a valid variable name
     *
     * @param name The name to check
     * @return Whether the name is a valid variable name
     */
    bool is_valid_variable(const std::string &name)
    {
        return boost::regex_match(name, _variable_name);
    }

    const boost::regex _short_option_name = boost::regex(R"(^-[a-zA-Z]$)");

    /**
     * @brief Whether a string is a valid short option name
     *
     * @param name The name to check
     * @return Whether the name is a valid short option name
     */
    bool is_valid_short_option(const std::string &name)
    {
        return boost::regex_match(name, _short_option_name);
    }

    const boost::regex _long_option_name = boost::regex(R"(^--[a-zA-Z\-_]+$)");

    /**
     * @brief Whether a string is a valid long option name
     *
     * @param name The name to check
     * @return Whether the name is a valid long option name
     */
    bool is_valid_long_option(const std::string &name)
    {
        return boost::regex_match(name, _long_option_name);
    }

    template <typename T>
    T sqrt(const T value)
    {
        if (value < 0)
        {
            throw std::out_of_range(format("Attempted to calculate square root of %lf < 0", value));
        }

        if (value == 0)
        {
            return 0;
        }

        T low = 0, high = std::max((T)1, value), accuracy = 1;
        if constexpr (std::is_floating_point_v<T>)
        {
            accuracy = 1.0e-6;
        }

        while (high - low > accuracy)
        {
            double mid = (low + high) / 2;
            if (mid * mid < value)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
        }

        return high;
    }
}
