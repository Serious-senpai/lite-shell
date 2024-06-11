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
     * @brief Get the absolute path using the native method
     * [`GetFullPathNameW`](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew)
     *
     * @param path The path to get the absolute path of
     * @return The absolute path
     */
    std::string get_absolute_path(const std::string &path)
    {
        wchar_t buffer[MAX_PATH];
        auto size = GetFullPathNameW(utf_convert(path).c_str(), MAX_PATH, buffer, NULL);
        if (size == 0)
        {
            throw std::runtime_error(last_error("GetFullPathNameW ERROR"));
        }

        return utf_convert(std::wstring(buffer, buffer + size));
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

    /**
     * @brief List all files matching a specific pattern (typically used to list a directory)
     * @see https://stackoverflow.com/a/24193730
     */
    std::vector<WIN32_FIND_DATAW> list_files(const std::string &__pattern)
    {
        std::vector<WIN32_FIND_DATAW> results(1);

        HANDLE file = FindFirstFileW(utf_convert(__pattern).c_str(), &results[0]);
        if (file == INVALID_HANDLE_VALUE)
        {
            return {};
        }

        do
        {
            results.emplace_back();
        } while (FindNextFileW(file, &results.back()));

        results.pop_back();

        if (!FindClose(file))
        {
            throw std::runtime_error(last_error("Error when closing file search handle"));
        }

        CloseHandle(file);

#ifdef DEBUG
        std::vector<std::string> names;
        for (const auto &data : results)
        {
            names.push_back(utf_convert(std::wstring(data.cFileName)));
        }

        std::cout << "Items matching \"" << __pattern << "\":" << names << std::endl;
#endif

        return results;
    }

    /**
     * @brief Remove a directory recursively. It is the caller's responsibility to ensure that the directory exists.
     *
     * @param directory The directory to remove
     * @param verbose Whether to display the deletion progress
     * @return Whether the directory was removed successfully
     */
    bool remove_directory(const std::string &directory, bool verbose)
    {
        bool success = true;
        for (const auto &file : list_files(join(directory, "*")))
        {
            auto filename = utf_convert(file.cFileName);

            // Skip current and parent directory
            if (filename == "." || filename == "..")
            {
                continue;
            }

            auto path = join(directory, filename);
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!remove_directory(path, verbose))
                {
                    success = false;
                }
            }
            else if (DeleteFileW(utf_convert(path).c_str()))
            {
                std::cout << "Deleted " << path << std::endl;
            }
            else
            {
                std::cerr << last_error(format("Error deleting file \"%s\"", path.c_str())) << std::endl;
                success = false;
            }
        }

        if (RemoveDirectoryW(utf_convert(directory).c_str()))
        {
            std::cout << "Deleted " << directory << std::endl;
        }
        else
        {
            std::cerr << last_error(format("Error deleting directory \"%s\"", directory.c_str())) << std::endl;
            success = false;
        }

        return success;
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

    /**
     * @brief Escape a string to obtain a valid regex pattern
     *
     * @param original The original string
     * @return The escaped string
     */
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

    /**
     * @brief Get the status of global memory using the native method
     * [`GlobalMemoryStatusEx`](https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex)
     *
     * @return The status of global memory
     */
    MEMORYSTATUSEX get_global_memory_status()
    {
        MEMORYSTATUSEX memory_status;
        memory_status.dwLength = sizeof(memory_status);
        if (!GlobalMemoryStatusEx(&memory_status))
        {
            throw std::runtime_error(last_error("GlobalMemoryStatusEx ERROR"));
        }

        return memory_status;
    }

    /**
     * @brief Calculate the square root of a value using binary search
     *
     * @param value The value to calculate the square root of
     * @return The square root of the value
     */
    template <typename T>
    T sqrt(const T &value)
    {
        if (value < 0)
        {
            throw std::out_of_range(format("Attempted to calculate square root of %s < 0", std::to_string(value).c_str()));
        }

        if (value == 0)
        {
            return 0;
        }

        T low = 0, high = std::max(static_cast<T>(1), value), accuracy = 1;
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

    /** @brief Convert an integer to its hex representation */
    template <typename T>
    std::string to_hex_string(const T &value)
    {
        static_assert(std::is_integral_v<T>, "to_hex_string called with a non-integral type");
        std::stringstream stream;
        stream << std::hex << value;
        return stream.str();
    }

    /**
     * @brief Convert a string to lowercase
     *
     * @param str The string to convert
     * @return The lowercase version of the string
     */
    std::string to_lowercase(const std::string &str)
    {
        std::string result(str);
        std::transform(result.begin(), result.end(), result.begin(), tolower);
        return result;
    }

    void hexToRgb(const std::string &hex, int &r, int &g, int &b)
    {
        std::stringstream ss;
        ss << std::hex << hex.substr(1);
        unsigned int hexColor;
        ss >> hexColor;
        r = (hexColor >> 16) & 0xFF;
        g = (hexColor >> 8) & 0xFF;
        b = hexColor & 0xFF;
    }

    // Validate hex color code
    bool isValidHexColor(const std::string &hexColor)
    {
        if (hexColor.size() != 7 || hexColor[0] != '#')
        {
            return false;
        }
        // Regular expression to match valid hex color code
        std::regex hexPattern("#[0-9a-fA-F]{6}");
        return std::regex_match(hexColor, hexPattern);
    }

    // Set console text color using RGB values
    void setColor(int r, int g, int b)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFOEX info;
        info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        GetConsoleScreenBufferInfoEx(hConsole, &info);

        // Modify color table at index 1
        info.ColorTable[10] = RGB(r, g, b);

        SetConsoleScreenBufferInfoEx(hConsole, &info);
        SetConsoleTextAttribute(hConsole, 10);
    }

    // Wrapper function to set color using hex code
    bool setColor(const std::string &hexColor)
    {
        if (!isValidHexColor(hexColor))
        {
            throw std::invalid_argument("Error: Invalid hex color format " + hexColor);
            return false;
        }

        int r, g, b;
        hexToRgb(hexColor, r, g, b);
        setColor(r, g, b);
        return true;
    }
}
