#pragma once

#include <cctype>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <regex>

#include <pathcch.h>
#include <windows.h>
#include <wininet.h>

#include <boost/regex.hpp>

namespace std
{
    /**
     * @brief This does exactly what you think it does
     *
     * @param _x The first value
     * @param _y The second value
     * @param _z The third value
     * @return The minimum of the 3 values
     */
    template <typename T>
    const T &min(const T &_x, const T &_y, const T &_z)
    {
        return min(_x, min(_y, _z));
    }

    /**
     * @brief This does exactly what you think it does
     *
     * @param _x The first value
     * @param _y The second value
     * @param _z The third value
     * @param _a The fourth value
     * @return The minimum of the 4 values
     */
    template <typename T>
    const T &min(const T &_x, const T &_y, const T &_z, const T &_a)
    {
        return min(_x, min(_y, _z, _a));
    }

    template <typename _ForwardIterator>
    void __list_elements(ostream &stream, const _ForwardIterator &_begin, const _ForwardIterator &_end)
    {
        for (auto iter = _begin; iter != _end; iter++)
        {
            stream << *iter;
            if (next(iter) != _end)
            {
                stream << ", ";
            }
        }
    }

    template <typename T>
    ostream &operator<<(ostream &stream, const vector<T> &_v)
    {
        stream << "[";
        __list_elements(stream, _v.begin(), _v.end());
        stream << "]";

        return stream;
    }

    template <typename T>
    ostream &operator<<(ostream &stream, const list<T> &_l)
    {
        stream << "[";
        __list_elements(stream, _l.begin(), _l.end());
        stream << "]";

        return stream;
    }

    template <typename T, typename Compare, typename Alloc>
    ostream &operator<<(ostream &stream, const set<T, Compare, Alloc> &_s)
    {
        stream << "{";
        __list_elements(stream, _s.begin(), _s.end());
        stream << "}";

        return stream;
    }

    template <typename T, typename Compare, typename Alloc>
    ostream &operator<<(ostream &stream, const multiset<T, Compare, Alloc> &_s)
    {
        stream << "{";
        __list_elements(stream, _s.begin(), _s.end());
        stream << "}";

        return stream;
    }

    template <typename T1, typename T2, typename Compare, typename Alloc>
    ostream &operator<<(ostream &stream, const map<T1, T2, Compare, Alloc> &_m)
    {
        vector<pair<T1, T2>> _v(_m.begin(), _m.end());

        stream << "{";
        for (auto iter = _m.begin(); iter != _m.end(); iter++)
        {
            stream << iter->first << ": " << iter->second;
            if (next(iter) != _m.end())
            {
                stream << ", ";
            }
        }
        stream << "}";

        return stream;
    }

    template <typename T1, typename T2>
    ostream &operator<<(ostream &stream, const pair<T1, T2> &_p)
    {
        stream << "<" << _p.first << ", " << _p.second << ">";
        return stream;
    }
}
