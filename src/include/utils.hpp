#pragma once

#include "converter.hpp"
#include "standard.hpp"

std::string format_last_error(const std::string &message)
{
    return format("%s: %d", message.c_str(), GetLastError());
}

std::string get_working_directory()
{
    wchar_t buffer[MAX_PATH];
    auto size = GetCurrentDirectoryW(MAX_PATH, buffer);
    if (size == 0)
    {
        throw std::runtime_error(format_last_error("GetCurrentDirectoryW ERROR"));
    }

    return utf_convert(std::wstring(buffer, buffer + size));
}

std::string get_executable_path()
{
    wchar_t buffer[MAX_PATH];
    auto size = GetModuleFileNameW(NULL, buffer, MAX_PATH);
    if (size == 0)
    {
        throw std::runtime_error(format_last_error("Error calling GetModuleFileNameW"));
    }

    return utf_convert(std::wstring(buffer, buffer + size));
}

std::string join(const std::vector<std::string> &tokens)
{
    std::string result;
    if (!tokens.empty())
    {
        for (unsigned i = 0; i < tokens.size() - 1; i++)
        {
            result += tokens[i];
            result += ' ';
        }
        result += tokens.back();
    }

    return result;
}

std::string strip(const std::string &original)
{
    std::deque<char> result(original.begin(), original.end());
    while (!result.empty() && result.front() == ' ')
    {
        result.pop_front();
    }

    while (!result.empty() && (result.back() == ' ' || result.back() == '\n' || result.back() == '\r'))
    {
        result.pop_back();
    }

    return std::string(result.begin(), result.end());
}

std::vector<std::string> split(const std::string &original)
{
    auto wstr = utf_convert(original);

    int size = 0;
    auto results = CommandLineToArgvW(wstr.c_str(), &size);
    if (results == NULL)
    {
        throw std::runtime_error(format_last_error("CommandLineToArgvW ERROR"));
    }

    std::vector<std::string> args(size);
    for (int i = 0; i < size; i++)
    {
        args[i] = utf_convert(std::wstring(results[i]));
    }

    return args;
}

std::string join(std::string first, std::string second)
{
    {
        auto size = first.size();
        while (size > 0 && first[size - 1] == '\\')
        {
            size--;
        }
        first = first.substr(0, size);
    }

    {
        unsigned index = 0;
        while (index < second.size() - 1 && second[index] == '\\')
        {
            index++;
        }
        second = second.substr(index);
    }

    return first + '\\' + second;
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
            throw std::runtime_error(format_last_error("Error when listing directory"));
        }
    }

    do
    {
        results.emplace_back();
    } while (FindNextFileW(h_file, &results.back()));

    results.pop_back();

    if (!FindClose(h_file))
    {
        throw std::runtime_error(format_last_error("Error when closing file search handle"));
    }

    CloseHandle(h_file);
    return results;
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

template <typename _ForwardIterator>
_ForwardIterator fuzzy_search(_ForwardIterator first, _ForwardIterator last, const std::string &value)
{
    int n, m;
    std::string compare;
    std::vector<std::vector<int>> dp;

    std::function<int(int, int)> distance;
    distance = [&value, &n, &m, &compare, &dp, &distance](int value_index, int compare_index)
    {
        if (dp[value_index][compare_index] > -1)
        {
            return dp[value_index][compare_index];
        }

        int result = -1;
        if (value_index == n)
        {
            result = m - compare_index;
        }
        else if (compare_index == m)
        {
            result = n - value_index;
        }
        else if (value[value_index] == compare[compare_index])
        {
            result = distance(value_index + 1, compare_index + 1);
        }
        else
        {
            result = 1 + std::min(
                             distance(value_index, compare_index + 1),
                             distance(value_index + 1, compare_index),
                             distance(value_index + 1, compare_index + 1));
        }

        return dp[value_index][compare_index] = result;
    };

    int min_diff = INT_MAX;
    _ForwardIterator result;
    for (auto iter = first; iter != last; iter++)
    {
        compare = *iter;
        n = value.size();
        m = compare.size();
        dp.clear();
        dp.resize(n + 1, std::vector<int>(m + 1, -1));

        int diff = distance(0, 0);
        if (diff < min_diff)
        {
            min_diff = diff;
            result = iter;
        }
    }

    return result;
}

std::string ngettext(const bool predicate, const std::string &first, const std::string &second)
{
    return predicate ? first : second;
}