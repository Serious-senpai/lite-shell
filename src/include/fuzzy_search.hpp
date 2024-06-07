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
    _ForwardIterator fuzzy_search(const _ForwardIterator &first, const _ForwardIterator &last, const std::string &value);
}
