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
     * @brief Join 2 Windows paths using the native method
     * [`PathCchCombine`](https://learn.microsoft.com/en-us/windows/win32/api/pathcch/nf-pathcch-pathcchcombine)
     *
     * @param first The first path
     * @param second The second path
     * @return The joined path
     */
    std::string join(const std::string &first, const std::string &second)
    {
        wchar_t result[MAX_PATH];
        if (PathCchCombine(result, MAX_PATH, utils::utf_convert(first).c_str(), utils::utf_convert(second).c_str()) != S_OK)
        {
            throw std::invalid_argument("Cannot join the paths");
        }

        return utils::utf_convert(result);
    }
}