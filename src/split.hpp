#pragma once

#include "standard.hpp"

std::vector<std::string> split(const std::string &original)
{
    std::vector<std::string> result;
    result.emplace_back();

    for (auto &c : original)
    {
        if (c == ' ')
        {
            if (result.empty() || result.back().size() > 0)
            {
                result.emplace_back();
            }
        }
        else
        {
            result.back().push_back(c);
        }
    }

    if (result.back().size() == 0)
    {
        result.pop_back();
    }

    return result;
}
