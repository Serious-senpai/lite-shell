#pragma once

#include "format.hpp"
#include "standard.hpp"

class ParseResult
{
private:
    const std::vector<std::vector<std::string>> groups;
    const std::map<std::string, unsigned> results;

    static std::vector<std::set<std::string>> get_alias_group(
        const std::vector<std::vector<std::string>> &groups,
        const std::map<std::string, unsigned> &results)
    {
        std::vector<std::set<std::string>> result(groups.size());
        for (const auto &[name, index] : results)
        {
            result[index].insert(name);
        }

        return result;
    }

public:
    const std::vector<std::string> positional_arguments;
    const std::vector<std::set<std::string>> alias_groups;

    ParseResult(
        const std::vector<std::vector<std::string>> &groups,
        const std::map<std::string, unsigned> &results,
        const std::vector<std::string> positional_arguments)
        : groups(groups),
          results(results),
          positional_arguments(positional_arguments),
          alias_groups(get_alias_group(groups, results)) {}

    const std::vector<std::string> &get(const std::string &name) const
    {
        if (!results.count(name))
        {
            throw std::invalid_argument(format("Unknown argument: %s", name));
        }

        auto index = results.at(name);
        return groups[index];
    }
};
