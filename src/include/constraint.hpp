#pragma once

#include "format.hpp"
#include "standard.hpp"

/* Represents the argument constraints for a command */
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
    /* Whether this command require the context to be parsed */
    const bool require_context_parsing;

    /*
    Whether to perform arguments checking (check positional and named arguments) on command input. If `require_context_parsing`
    is `false`, this attribute has no effect.
    */
    const bool arguments_checking;

    /*
    The lower and upper bound of the number of positional arguments. If `require_context_parsing` or `arguments_checking` is
    `false`, this attribute has no effect.
    */
    const std::pair<unsigned, unsigned> args_bounds;

    /* Construct an `ArgumentsConstraint` object with `require_context_parsing` set to `false` */
    ArgumentsConstraint() : ArgumentsConstraint(false, false, std::make_pair(0, 0)) {}

    /*
    Construct an `ArgumentsConstraint` object with `require_context_parsing` set to `true` and `arguments_checking` set to
    `false`.

    @param arguments_checking Must be `false`
    */
    ArgumentsConstraint(const bool arguments_checking) : ArgumentsConstraint(true, arguments_checking, std::make_pair(0, 0))
    {
        if (arguments_checking)
        {
            throw std::invalid_argument("This overload does not allow arguments checking");
        }
    }

    /*
    Construct an `ArgumentsConstraint` object with `require_context_parsing` and `arguments_checking` set to `true`.

    @param args_bounds The lower and upper bound of the number of positional arguments
    */
    ArgumentsConstraint(const std::pair<unsigned, unsigned> &args_bounds) : ArgumentsConstraint(true, true, args_bounds) {}

    /*
    Construct an `ArgumentsConstraint` object with `require_context_parsing` and `arguments_checking` set to `true`.

    @param args_lower The lower bound of the number of positional arguments
    @param args_upper The upper bound of the number of positional arguments
    */
    ArgumentsConstraint(const unsigned args_lower, const unsigned args_upper)
        : ArgumentsConstraint(true, true, std::make_pair(args_lower, args_upper)) {}

    /* Throw an error if this constraint has `require_context_parsing` set to `false` */
    void check_context_parsing()
    {
        if (!require_context_parsing)
        {
            throw std::invalid_argument("ArgumentsConstraint object does not support context parsing");
        }
    }

    ArgumentsConstraint *add_argument(const std::string &__name, const std::string &__help)
    {
        check_context_parsing();
        if (has_argument(__name))
        {
            throw std::invalid_argument(format("Argument %s already exists", __name.c_str()));
        }

        names[__name] = help.size();
        help.push_back(__help);
        return this;
    }

    template <typename... Args>
    ArgumentsConstraint *add_argument(const Args &...aliases, const std::string &help)
    {
        check_context_parsing();
        std::set<std::string> alias_group = {aliases...};
        alias_groups.insert(alias_group);
        for (auto &alias : {aliases...})
        {
            add_argument(alias, help);
        }

        return this;
    }

    bool has_argument(const std::string &name) const
    {
        return names.count(name);
    }

    std::string get_help(const std::string &name) const
    {
        auto iter = names.find(name);
        if (iter == names.end())
        {
            throw std::invalid_argument(format("Argument %s does not exist", name.c_str()));
        }

        return help[iter->second];
    }

    std::set<std::set<std::string>> get_alias_groups() const
    {
        return alias_groups;
    }
};
