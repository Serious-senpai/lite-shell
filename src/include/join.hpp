#pragma once

#include "standard.hpp"

namespace utils
{
    /**
     * @brief Join multiple strings seperated by a delimiter
     *
     * @param __begin An iterator pointing to the first string
     * @param __end An iterator pointing after the last string
     * @param delimiter The delimiter to use
     * @return The joined string
     */
    template <typename _ForwardIterator>
    std::string join(
        const _ForwardIterator &__begin,
        const _ForwardIterator &__end,
        const std::string &delimiter);

    /**
     * @brief Join 2 Windows paths
     *
     * @param first The first path
     * @param second The second path
     * @return The joined path
     */
    std::string join(std::string first, std::string second);
}
