// Source: https://stackoverflow.com/a/18374698
#pragma once

/**
 * Convert `std::string` to `std::wstring`
 *
 * @param str An `std::string` to convert
 * @return The converted `std::wstring`
 */
std::wstring utf_convert(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

/**
 * Convert `std::wstring` to `std::wstring`
 *
 * @param wstr An `std::wstring` to convert
 * @return The converted `std::string`
 */
std::string utf_convert(const std::wstring &wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}