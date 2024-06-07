#pragma once

#include "format.hpp"

namespace utils
{
    /**
     * @brief Formatter for printing memory sizes, e.g. 1024 bytes -> `1.00KB`
     *
     * @param bytes The number of bytes to format, e.g. 1024
     * @return The formatted string, e.g. `1.00KB`
     */
    std::string memory_size(long double bytes);
}
