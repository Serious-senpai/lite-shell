#pragma once

#include "utils.hpp"

namespace utils
{
    /** @brief Split a string into tokens using the native method CommandLineToArgvW */
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

    /** @brief Split a string into tokens using a delimiter */
    std::vector<std::string> split(const std::string &original, const char delimiter)
    {
        std::vector<std::string> result;
        bool create_new = true;
        for (auto c : original)
        {
            if (c == delimiter)
            {
                create_new = true;
                continue;
            }

            if (create_new)
            {
                result.push_back("");
                create_new = false;
            }

            result.back().push_back(c);
        }

        return result;
    }
}