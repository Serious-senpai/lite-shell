#pragma once

#include "standard.hpp"

std::vector<std::string> split(const std::string &original)
{
    std::vector<std::string> result;
    result.emplace_back();

    bool in_single_quotes = false, in_double_quotes = false, escape = false;
    for (auto &c : original)
    {
        if (c == '\'' && !escape && !in_double_quotes)
        {
            in_single_quotes = !in_single_quotes;
        }
        else if (c == '"' && !escape && !in_single_quotes)
        {
            in_double_quotes = !in_double_quotes;
        }
        else if (c == ' ' && !in_single_quotes && !in_double_quotes)
        {
            if (result.empty() || result.back().size() > 0)
            {
                result.emplace_back();
            }
        }
        else
        {
            if (c == '\\')
            {
                if (!escape)
                {
                    escape = true;
                    continue;
                }
            }
            else
            {
                escape = false;
            }

            result.back().push_back(c);
        }
    }

    if (result.back().size() == 0)
    {
        result.pop_back();
    }

    return result;
}
