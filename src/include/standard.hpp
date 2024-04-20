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
    /**
     * @brief This does exactly what you think it does
     *
     * @param _x The first value
     * @param _y The second value
     * @param _z The third value
     * @return The minimum of the three values
     */
    template <typename T>
    const T &min(const T &_x, const T &_y, const T &_z)
    {
        return std::min(_x, std::min(_y, _z));
    }
}

template <typename _ForwardIterator>
void _list_elements(std::ostream &stream, const _ForwardIterator &_begin, const _ForwardIterator &_end)
{
    for (auto iter = _begin; iter != _end; iter++)
    {
        stream << *iter;
        if (std::next(iter) != _end)
        {
            stream << ", ";
        }
    }
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::vector<T> &_v)
{
    stream << "[";
    _list_elements(stream, _v.begin(), _v.end());
    stream << "]";

    return stream;
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::list<T> &_l)
{
    stream << "[";
    _list_elements(stream, _l.begin(), _l.end());
    stream << "]";

    return stream;
}

template <typename T, typename Compare, typename Alloc>
std::ostream &operator<<(std::ostream &stream, const std::set<T, Compare, Alloc> &_s)
{
    stream << "{";
    _list_elements(stream, _s.begin(), _s.end());
    stream << "}";

    return stream;
}

template <typename T, typename Compare, typename Alloc>
std::ostream &operator<<(std::ostream &stream, const std::multiset<T, Compare, Alloc> &_s)
{
    stream << "{";
    _list_elements(stream, _s.begin(), _s.end());
    stream << "}";

    return stream;
}

template <typename T1, typename T2, typename Compare, typename Alloc>
std::ostream &operator<<(std::ostream &stream, const std::map<T1, T2, Compare, Alloc> &_m)
{
    std::vector<std::pair<T1, T2>> _v(_m.begin(), _m.end());

    stream << "{";
    for (auto iter = _m.begin(); iter != _m.end(); iter++)
    {
        stream << iter->first << ": " << iter->second;
        if (std::next(iter) != _m.end())
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
