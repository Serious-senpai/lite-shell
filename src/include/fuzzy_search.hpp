#pragma once

#include "standard.hpp"

namespace utils
{
    /**
     * @brief Search for the closest matching string in a given range
     *
     * Search for a string in range [`first`, `last`) that is closest to `value`
     *
     * @param first An iterator pointing to the first string
     * @param last An iterator pointing after last string
     * @param value The value to search for
     *
     * @return An iterator pointing to the string that is closest to `value`
     */
    template <typename _ForwardIterator>
    _ForwardIterator fuzzy_search(const _ForwardIterator &first, const _ForwardIterator &last, const std::string &value)
    {
        if (first == last)
        {
            throw std::invalid_argument("fuzzy_search got an empty range");
        }

        int n, m;
        std::string compare;
        std::vector<std::vector<int>> dp;

        std::function<int(int, int)> distance;
        distance = [&value, &n, &m, &compare, &dp, &distance](int value_index, int compare_index)
        {
            if (dp[value_index][compare_index] > -1)
            {
                return dp[value_index][compare_index];
            }

            int result = -1;
            if (value_index == n)
            {
                result = m - compare_index;
            }
            else if (compare_index == m)
            {
                result = n - value_index;
            }
            else if (value[value_index] == compare[compare_index])
            {
                result = distance(value_index + 1, compare_index + 1);
            }
            else
            {
                result = 1 + std::min(
                                 distance(value_index, compare_index + 1),
                                 distance(value_index + 1, compare_index),
                                 distance(value_index + 1, compare_index + 1));
            }

            return dp[value_index][compare_index] = result;
        };

        int min_diff = INT_MAX;
        _ForwardIterator result;
        for (auto iter = first; iter != last; iter++)
        {
            compare = *iter;
            n = value.size();
            m = compare.size();
            dp.clear();
            dp.resize(n + 1, std::vector<int>(m + 1, -1));

            int diff = distance(0, 0);
            if (diff < min_diff)
            {
                min_diff = diff;
                result = iter;
            }
        }

        return result;
    }
}
