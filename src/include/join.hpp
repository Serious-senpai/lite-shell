#pragma once

namespace utils
{
    /**
     * @brief Join multiple strings seperated by a delimiter
     *
     * @param __begin An iterator pointing to the first string
     * @param __end An iterator pointing after the last string
     * @param delimiter The delimiter to use
     * @return The joined string
     */
    template <typename _ForwardIterator>
    std::string join(
        const _ForwardIterator &__begin,
        const _ForwardIterator &__end,
        const std::string &delimiter)
    {
        std::string result;
        for (auto iter = __begin; iter != __end; iter++)
        {
            result += *iter;

            auto peek = iter;
            if (++peek != __end)
            {
                result += delimiter;
            }
        }

        return result;
    }

    /**
     * @brief Join 2 Windows paths
     *
     * @param first The first path
     * @param second The second path
     * @return The joined path
     */
    std::string join(std::string first, std::string second)
    {
        {
            auto size = first.size();
            while (size > 0 && first[size - 1] == '\\')
            {
                size--;
            }
            first = first.substr(0, size);
        }

        {
            unsigned index = 0;
            while (index < second.size() - 1 && second[index] == '\\')
            {
                index++;
            }
            second = second.substr(index);
        }

        return first + '\\' + second;
    }
}