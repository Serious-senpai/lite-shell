#pragma once

#include "converter.hpp"
#include "standard.hpp"

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
