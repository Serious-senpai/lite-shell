#pragma once

#include "utils.hpp"

namespace utils
{
    /**
     * @brief A mapping of case-insensitive strings to values
     *
     * @tparam V The value type
     */
    template <typename V>
    class CaseInsensitiveMap
    {
    private:
        typedef typename std::map<std::string, V> _map_type;

        _map_type _map;

    public:
        /** @brief A bidirectional iterator to `std::pair<std::string, V>` */
        typedef typename _map_type::iterator iterator;
        /** @brief A bidirectional iterator to `const std::pair<std::string, V>` */
        typedef typename _map_type::const_iterator const_iterator;

        /** @brief Access element */
        V &operator[](const std::string &key)
        {
            return _map[to_lowercase(key)];
        }

        /** @brief Return iterator to beginning */
        iterator begin()
        {
            return _map.begin();
        }

        /** @brief Return const_iterator to beginning */
        const_iterator begin() const
        {
            return _map.begin();
        }

        /** @brief Return iterator to end */
        iterator end()
        {
            return _map.end();
        }

        /** @brief Return const_iterator to end */
        const_iterator end() const
        {
            return _map.end();
        }

        /** @brief Return const_iterator to beginning */
        const_iterator cbegin() const
        {
            return _map.cbegin();
        }

        /** @brief Return const_iterator to end */
        const_iterator cend() const
        {
            return _map.cend();
        }

        /** @brief Get iterator to element */
        iterator find(const std::string &key)
        {
            return _map.find(to_lowercase(key));
        }

        /** @brief Get const_iterator to element */
        const_iterator find(const std::string &key) const
        {
            return _map.find(to_lowercase(key));
        }
    };
}
