#pragma once

/* Join multiple strings seperated by a delimiter */
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

/* Join 2 Windows paths */
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
