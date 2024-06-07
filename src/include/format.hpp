#pragma once

#include "standard.hpp"

namespace utils
{
    /**
     * @brief Format a string with C specifiers.
     * @see https://stackoverflow.com/a/26221725
     *
     * @tparam ...Args The format arguments
     * @param format The format string
     * @param args The arguments to format
     * @return The formatted string
     */
    template <typename... Args>
    std::string format(const std::string &format, const Args &...args);
}
