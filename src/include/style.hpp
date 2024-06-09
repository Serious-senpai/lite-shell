#pragma once

#include "standard.hpp"

namespace utils
{
    template <typename T>
    void style_print(const T &message, const WORD &attributes)
    {
        const auto console = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO current;
        const auto has_old_attr = GetConsoleScreenBufferInfo(console, &current);

        SetConsoleTextAttribute(console, attributes); // Ignore failure
        std::cout << message;

        if (has_old_attr)
        {
            SetConsoleTextAttribute(console, current.wAttributes);
        }
    }
}
