#pragma once

#include <codecvt>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stack>
#include <windows.h>

#include <boost/regex.hpp>

namespace std
{
    template <typename T>
    const T &min(const T &_x, const T &_y, const T &_z)
    {
        return std::min(_x, std::min(_y, _z));
    }
}