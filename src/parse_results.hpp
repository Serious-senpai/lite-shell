#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "format.hpp"

class ParseResult
{
private:
    const std::vector<std::vector<std::string>> groups;
    const std::map<std::string, std::vector<std::vector<std::string>>::iterator> results;

public:
    const std::vector<std::string> positional_arguments;

    ParseResult(
        const std::vector<std::vector<std::string>> &groups,
        const std::map<std::string, std::vector<std::vector<std::string>>::iterator> &results,
        const std::vector<std::string> positional_arguments)
        : groups(groups), results(results), positional_arguments(positional_arguments) {}

    std::vector<std::string> get(const std::string &name)
    {
        if (!results.count(name))
        {
            throw std::invalid_argument(format("Unknown argument: %s", name));
        }

        return *results.at(name);
    }
};
