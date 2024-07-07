#pragma once

#include "standard.hpp"

namespace utils
{
    std::mt19937 _rng(std::chrono::steady_clock::now().time_since_epoch().count());

    /**
     * @brief Generate a random number
     *
     * @tparam T The number type (`int`, `unsigned`, `double`, ...)
     * @param l The lower bound (inclusive) of the range
     * @param r The upper bound (inclusive) of the range
     * @return A random number in range [l, r]
     */
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T random(const T &l, const T &r)
    {
        std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>> unif(l, r);
        return unif(_rng);
    }

    const std::string ALPHANUMERIC = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    /**
     * @brief Generate a random string
     *
     * @param length The length of the string
     * @return A random string of the given length
     */
    std::string random(const std::size_t length)
    {
        std::uniform_int_distribution<std::size_t> unif(0, ALPHANUMERIC.size() - 1);
        std::stringstream result;
        for (std::size_t i = 0; i < length; i++)
        {
            result << ALPHANUMERIC[unif(_rng)];
        }
        return result.str();
    }
}
