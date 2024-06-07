#pragma once

#include "utils.hpp"

namespace utils
{
    /**
     * @brief Split a string into tokens using the native method [`CommandLineToArgvW`](https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw)
     *
     * @param original The string to split
     * @return A vector of strings containing the tokens
     */
    std::vector<std::string> split(const std::string &original);

    /** @brief Split a string into tokens using a delimiter */
    std::vector<std::string> split(const std::string &original, const char delimiter);
}
