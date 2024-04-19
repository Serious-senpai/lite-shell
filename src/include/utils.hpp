#pragma once

#include "converter.hpp"
#include "join.hpp"

namespace utils
{
    std::string last_error(const std::string &message)
    {
        return format("%s: %d", message.c_str(), GetLastError());
    }

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

    BOOL WINAPI ctrl_handler(DWORD ctrl_type)
    {
        return ctrl_type == CTRL_C_EVENT;
    }

    void set_ignore_ctrl_c(bool ignore)
    {
        if (!SetConsoleCtrlHandler(ctrl_handler, ignore))
        {
            std::cerr << last_error("Warning: SetConsoleCtrlHandler ERROR") << std::endl;
        }
    }

    std::vector<WIN32_FIND_DATAW> explore_directory(const std::string &__directory, const std::string &__pattern = "\\*")
    {
        auto directory = join(__directory, __pattern);
        std::vector<WIN32_FIND_DATAW> results(1);

        HANDLE h_file = FindFirstFileW(utf_convert(directory).c_str(), &results[0]);
        if (h_file == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) // No file with the specified pattern was found
            {
                return {};
            }
            else
            {
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

    bool startswith(const std::string &string, const std::string &prefix)
    {
        if (string.size() < prefix.size())
        {
            return false;
        }

        for (unsigned i = 0; i < prefix.size(); i++)
        {
            if (string[i] != prefix[i])
            {
                return false;
            }
        }

        return true;
    }

    bool endswith(const std::string &string, const std::string &suffix)
    {
        if (string.size() < suffix.size())
        {
            return false;
        }

        for (unsigned i = string.size() - suffix.size(); i < string.size(); i++)
        {
            if (string[i] != suffix[i + suffix.size() - string.size()])
            {
                return false;
            }
        }

        return true;
    }

    bool is_executable(LPCWSTR name)
    {
        DWORD _;
        return GetBinaryTypeW(name, &_);
    }

    std::string ngettext(const bool predicate, const std::string &first, const std::string &second)
    {
        return predicate ? first : second;
    }

    bool is_math_symbol(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == ' ' || c == '%' || ('0' <= c && c <= '9') || c == '(' || c == ')';
    }

    const boost::regex _command_name = boost::regex(R"(^\w+$)");
    bool is_valid_command(const std::string &name)
    {
        return boost::regex_match(name, _command_name);
    }

    const boost::regex _variable_name = boost::regex(R"(^\w+$)");
    bool is_valid_variable(const std::string &name)
    {
        return boost::regex_match(name, _variable_name);
    }

    const boost::regex _short_option_name = boost::regex(R"(^-[a-zA-Z]$)");
    bool is_valid_short_option(const std::string &name)
    {
        return boost::regex_match(name, _short_option_name);
    }

    const boost::regex _long_option_name = boost::regex(R"(^--[a-zA-Z\-_]+$)");
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

    std::vector<long long> range(const long long start, const long long end)
    {
        std::vector<long long> result;
        for (long long i = start; i < end; i++)
        {
            result.push_back(i);
        }
        return result;
    }
}