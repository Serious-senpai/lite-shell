#pragma once

#include "standard.hpp"

namespace utils
{
    /**
     * @brief Remove characters from the beginning and ending of a string
     *
     * @param original The original string
     * @param remove The characters to remove
     * @return The stripped string
     */
    template <typename... Args>
    std::string strip(const std::string &original, const Args &...remove);

    /** @brief Remove spaces, newlines, and carriage returns from the beginning and ending of a string */
    std::string strip(const std::string &original);
}
