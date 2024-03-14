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
        throw std::runtime_error("Error getting current directory.");
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

    std::vector<std::string> args(size);
    for (int i = 0; i < size; i++)
    {
        args[i] = utf_convert(std::wstring(results[i]));
    }

    return args;
}

std::vector<WIN32_FIND_DATAW> explore_directory(const std::string &__directory)
{
    auto directory = __directory + "\\*";
    std::vector<WIN32_FIND_DATAW> results(1);

    HANDLE h_file = FindFirstFileW(utf_convert(directory).c_str(), &results[0]);
    if (h_file == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error(format_last_error("Error when listing directory"));
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