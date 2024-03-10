#pragma once

#include "standard.hpp"
// Source: https://stackoverflow.com/a/18374698

// Convert std::string to std::wstring
std::wstring utf_convert(const std::string &str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

// Convert std::wstring to std::string
std::string utf_convert(const std::wstring &wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}