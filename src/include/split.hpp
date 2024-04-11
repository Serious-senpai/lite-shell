#pragma once

#include "utils.hpp"

namespace utils
{
    /* Split a string into tokens using the native method CommandLineToArgvW */
    std::vector<std::string> split(const std::string &original)
    {
        auto wstr = utf_convert(original);

        int size = 0;
        auto results = CommandLineToArgvW(wstr.c_str(), &size);
        if (results == NULL)
        {
            throw std::runtime_error(last_error("CommandLineToArgvW ERROR"));
        }

        std::vector<std::string> args(size);
        for (int i = 0; i < size; i++)
        {
            args[i] = utf_convert(std::wstring(results[i]));
        }

        return args;
    }

    std::vector<std::string> split(const std::string &original, const char delimiter)
    {
        std::vector<std::string> result;
        std::istringstream stream(original);
        std::string token;
        while (std::getline(stream, token, delimiter))
        {
            result.push_back(token);
        }
        return result;
    }
}