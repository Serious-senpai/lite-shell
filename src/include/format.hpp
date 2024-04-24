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
    std::string format(const std::string &format, const Args &...args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        if (size_s <= 0)
        {
            throw std::runtime_error("Error during formatting.");
        }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
}
