#pragma once

#include <cassert>
#include <codecvt>
#include <deque>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>

namespace std
{
    template <typename T>
    const T &min(const T &_x, const T &_y, const T &_z)
    {
        return std::min(_x, std::min(_y, _z));
    }
}