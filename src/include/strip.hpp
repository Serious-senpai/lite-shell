#pragma once

#include "standard.hpp"

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
