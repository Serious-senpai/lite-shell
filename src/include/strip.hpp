#pragma once

#include "standard.hpp"

std::string strip(const std::string &original, const std::initializer_list<char> &remove)
{
    std::vector<char> result;
    std::set<char> to_remove(remove.begin(), remove.end());
    bool ok = false;
    for (auto c : original)
    {
        if (ok)
        {
            result.push_back(c);
        }
        else if (to_remove.count(c) == 0)
        {
            result.push_back(c);
            ok = true;
        }
    }

    while (!result.empty() && to_remove.count(result.back()) == 1)
    {
        result.pop_back();
    }

    return std::string(result.begin(), result.end());
}

std::string strip(const std::string &original)
{
    return strip(original, {' ', '\n', '\r'});
}
