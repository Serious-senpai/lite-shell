#pragma once

namespace utils
{
    /**
     * @brief Remove characters from the beginning and ending of a string
     *
     * @param original The original string
     * @param remove The characters to remove
     * @return The stripped string
     */
    template <typename... Args>
    std::string strip(const std::string &original, const Args &...remove)
    {
        std::vector<char> result;
        std::set<char> to_remove = {remove...};
        bool ok = false;
        for (auto c : original)
        {
            if (ok)
            {
                result.push_back(c);
            }
            else if (to_remove.count(c) == 0)
            {
                result.push_back(c);
                ok = true;
            }
        }

        while (!result.empty() && to_remove.count(result.back()) == 1)
        {
            result.pop_back();
        }

        return std::string(result.begin(), result.end());
    }

    /** @brief Remove spaces, newlines, and carriage returns from the beginning and ending of a string */
    std::string strip(const std::string &original)
    {
        return strip(original, ' ', '\n', '\r');
    }
}