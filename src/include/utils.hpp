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
