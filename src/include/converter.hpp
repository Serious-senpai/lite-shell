#pragma once

#include "standard.hpp"
// Source: https://stackoverflow.com/a/18374698

/*
Convert an std::string to std::wstring

@param str An std::string to convert
@return The converted std::wstring
*/
std::wstring utf_convert(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

/*
Convert an std::wstring to a std::wstring

@param wstr An std::wstring to convert
@return The converted std::string
*/
std::string utf_convert(const std::wstring &wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}