#pragma once

#include "standard.hpp"
// Source: https://stackoverflow.com/a/18374698

// Convert std::string to std::wstring
std::wstring utf_convert(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

// Convert std::wstring to std::string
std::string utf_convert(const std::wstring &wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}