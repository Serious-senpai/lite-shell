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

template <typename T>
void list_elements(std::ostream &stream, const std::vector<T> &_v)
{
    if (!_v.empty())
    {
        unsigned n = _v.size();
        for (unsigned i = 0; i < n; i++)
        {
            stream << _v[i];
            if (i < n - 1)
            {
                stream << ", ";
            }
        }
    }
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::vector<T> &_v)
{
    stream << "[";
    list_elements(stream, _v);
    stream << "]";

    return stream;
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::list<T> &_l)
{
    stream << "[";
    list_elements(stream, std::vector<T>(_l.begin(), _l.end()));
    stream << "]";

    return stream;
}

template <typename T, typename Compare, typename Alloc>
std::ostream &operator<<(std::ostream &stream, const std::set<T, Compare, Alloc> &_s)
{
    stream << "{";
    list_elements(stream, std::vector<T>(_s.begin(), _s.end()));
    stream << "}";

    return stream;
}

template <typename T, typename Compare, typename Alloc>
std::ostream &operator<<(std::ostream &stream, const std::multiset<T, Compare, Alloc> &_s)
{
    stream << "{";
    list_elements(stream, std::vector<T>(_s.begin(), _s.end()));
    stream << "}";

    return stream;
}

template <typename T1, typename T2, typename Compare, typename Alloc>
std::ostream &operator<<(std::ostream &stream, const std::map<T1, T2, Compare, Alloc> &_m)
{
    std::vector<std::pair<T1, T2>> _v(_m.begin(), _m.end());

    stream << "{";
    int n = _v.size();
    for (int i = 0; i < n; i++)
    {
        stream << _v[i].first << ": " << _v[i].second;
        if (i < n - 1)
        {
            stream << ", ";
        }
    }
    stream << "}";

    return stream;
}

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &stream, const std::pair<T1, T2> &_p)
{
    stream << "<" << _p.first << ", " << _p.second << ">";
    return stream;
}
