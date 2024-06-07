#include "converter.hpp"

namespace utils
{
    std::wstring utf_convert(const std::string &str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.from_bytes(str);
    }

    std::string utf_convert(const std::wstring &wstr)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wstr);
    }
}
