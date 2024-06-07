#pragma once

#include "standard.hpp"

namespace utils
{
    /**
     * @brief Convert `std::string` to `std::wstring`
     * @see https://stackoverflow.com/a/18374698
     * @param str An `std::string` to convert
     * @return The converted `std::wstring`
     */
    std::wstring utf_convert(const std::string &str);

    /**
     * @brief Convert `std::wstring` to `std::wstring`
     * @see https://stackoverflow.com/a/18374698
     * @param wstr An `std::wstring` to convert
     * @return The converted `std::string`
     */
    std::string utf_convert(const std::wstring &wstr);
}
