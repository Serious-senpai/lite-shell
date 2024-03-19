#pragma once

#include "format.hpp"
#include "standard.hpp"

class ArgumentsConstraint
{
private:
    std::vector<std::string> help;
    std::map<std::string, unsigned> names;
    std::set<std::set<std::string>> alias_groups;

    ArgumentsConstraint(
        const bool require_context_parsing,
        const bool arguments_checking,
        const std::pair<unsigned, unsigned> &args_bounds)
        : require_context_parsing(require_context_parsing),
          arguments_checking(arguments_checking),
          args_bounds(args_bounds)
    {
        if (require_context_parsing && arguments_checking)
        {
            if (args_bounds.first > args_bounds.second)
            {
                throw std::invalid_argument("Lower bound of positional arguments must be less than or equal to the upper bound");
            }

            if (args_bounds.first == 0)
            {
                throw std::invalid_argument("Lower bound of positional arguments must be at least 1");
            }
        }
    }

public:
    const bool require_context_parsing;
    const bool arguments_checking;
    const std::pair<unsigned, unsigned> args_bounds;

    ArgumentsConstraint() : ArgumentsConstraint(false, false, std::make_pair(0, 0)) {}
    ArgumentsConstraint(const bool arguments_checking) : ArgumentsConstraint(true, arguments_checking, std::make_pair(0, 0))
    {
        if (arguments_checking)
        {
            throw std::invalid_argument("This overload does not allow arguments checking");
        }
    }
    ArgumentsConstraint(const std::pair<unsigned, unsigned> &args_bounds) : ArgumentsConstraint(true, true, args_bounds) {}
    ArgumentsConstraint(const unsigned args_lower, const unsigned args_upper)
        : ArgumentsConstraint(true, true, std::make_pair(args_lower, args_upper)) {}

    void check_context_parsing()
    {
        if (!require_context_parsing)
        {
            throw std::invalid_argument("ArgumentsConstraint does not support context parsing");
        }
    }

    void add_argument(const std::string &__name, const std::string &__help)
    {
        check_context_parsing();
        if (has_argument(__name))
        {
            throw std::invalid_argument(format("Argument %s already exists", __name.c_str()));
        }

        names[__name] = help.size();
        help.push_back(__help);
    }

    template <typename... Args>
    void add_argument(const Args &...aliases, const std::string &help)
    {
        check_context_parsing();
        std::set<std::string> alias_group = {aliases...};
        alias_groups.insert(alias_group);
        for (auto &alias : {aliases...})
        {
            add_argument(alias, help);
        }
    }

    bool has_argument(const std::string &name) const
    {
        return names.count(name);
    }
};
