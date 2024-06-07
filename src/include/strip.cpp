#include "strip.hpp"

namespace utils
{
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

        while (!result.empty() && to_remove.find(result.back()) != to_remove.end())
        {
            result.pop_back();
        }

        return std::string(result.begin(), result.end());
    }

    std::string strip(const std::string &original)
    {
        return strip(original, ' ', '\n', '\r');
    }
}
