#pragma once

#include "converter.hpp"
#include "standard.hpp"

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

std::string join(int argc, const char **argv)
{
    std::vector<std::string> tokens;
    for (int i = 0; i < argc; i++)
    {
        tokens.emplace_back(argv[i]);
    }

    return join(tokens);
}