#pragma once

#include "format.hpp"

class ArgumentConstraint
{
public:
    const std::string help;
    const std::pair<unsigned, unsigned> bounds;
    const std::set<std::string> aliases;
    const bool required;

    ArgumentConstraint(
        const std::string &help,
        const unsigned lower_bound,
        const unsigned upper_bound,
        const std::set<std::string> &aliases,
        const bool required)
        : help(help), bounds(std::make_pair(lower_bound, upper_bound)), aliases(aliases), required(required)
    {
        if (lower_bound > upper_bound)
        {
            throw std::invalid_argument(format("lower_bound = %d > upper_bound = %d", lower_bound, upper_bound));
        }
    }
};

/* Represents the constraints for a command */
class CommandConstraint
{
private:
    std::vector<ArgumentConstraint> constraints;
    std::map<std::string, unsigned> names;

    CommandConstraint(
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

    template <typename... Args>
    CommandConstraint *add_argument(
        const bool required,
        const std::string &help,
        const unsigned lower_bound,
        const unsigned upper_bound,
        const std::string &name,
        const Args &...aliases)
    {
        check_context_parsing();

        std::set<std::string> alias_group = {name, aliases...};
        constraints.emplace_back(help, lower_bound, upper_bound, alias_group, required);

        for (auto &alias : alias_group)
        {
            if (has_argument(alias))
            {
                throw std::invalid_argument(format("Argument %s already exists", alias.c_str()));
            }

            names[alias] = constraints.size() - 1;
        }

        return this;
    }

public:
    /*
    @brief Whether this command require the context to be parsed. If this value if `false`, a context passed to the command callback
    will have `.args` and `.kwargs` set to empty containers.
    */
    const bool require_context_parsing;

    /*
    @brief Whether to perform arguments checking (check positional and named arguments) on command input. If `require_context_parsing`
    is `false`, this attribute has no effect.
    */
    const bool arguments_checking;

    /*
    The lower and upper bound of the number of positional arguments. If `require_context_parsing` or `arguments_checking` is
    `false`, this attribute has no effect.
    */
    const std::pair<unsigned, unsigned> args_bounds;

    /*
    @brief Construct an `CommandConstraint` object with `require_context_parsing` set to `false`.
    */
    CommandConstraint() : CommandConstraint(false, false, std::make_pair(0, 0)) {}

    /*
    @brief Construct an `CommandConstraint` object with `require_context_parsing` set to `true` and `arguments_checking` set to
    `false`.

    @param arguments_checking Must be `false`
    */
    CommandConstraint(const bool arguments_checking) : CommandConstraint(true, false, std::make_pair(0, 0))
    {
        if (arguments_checking)
        {
            throw std::invalid_argument("This overload does not allow arguments checking");
        }
    }

    /*
    @brief Construct an `CommandConstraint` object with `require_context_parsing` and `arguments_checking` set to `true`.

    @param args_lower The lower bound of the number of positional arguments
    @param args_upper The upper bound of the number of positional arguments
    */
    CommandConstraint(const unsigned args_lower, const unsigned args_upper)
        : CommandConstraint(true, true, std::make_pair(args_lower, args_upper)) {}

    /* Throw an error if this constraint has `require_context_parsing` set to `false` */
    void check_context_parsing()
    {
        if (!require_context_parsing)
        {
            throw std::invalid_argument("CommandConstraint object does not support context parsing");
        }
    }

    CommandConstraint *add_argument(
        const std::string &name,
        const bool required,
        const std::string &help,
        const unsigned lower_bound,
        const unsigned upper_bound)
    {
        return add_argument(required, help, lower_bound, upper_bound, name);
    }

    CommandConstraint *add_argument(
        const std::string &name,
        const std::string &alias_1,
        const bool required,
        const std::string &help,
        const unsigned lower_bound,
        const unsigned upper_bound)
    {
        return add_argument(required, help, lower_bound, upper_bound, name, alias_1);
    }

    CommandConstraint *add_argument(
        const std::string &name,
        const std::string &alias_1,
        const std::string &alias_2,
        const bool required,
        const std::string &help,
        const unsigned lower_bound,
        const unsigned upper_bound)
    {
        return add_argument(required, help, lower_bound, upper_bound, name, alias_1, alias_2);
    }

    /*
    @brief Whether the current constraint allows a specific named argument

    @param name The argument name (e.g. "-v")
    @return `true` if the argument is allowed, `false` otherwise
    */
    bool has_argument(const std::string &name) const
    {
        return names.count(name);
    }

    ArgumentConstraint get_constraint(const std::string &name) const
    {
        auto iter = names.find(name);
        if (iter == names.end())
        {
            throw std::invalid_argument(format("Argument %s does not exist", name.c_str()));
        }

        return constraints[iter->second];
    }

    std::set<std::set<std::string>> get_alias_groups() const
    {
        std::set<std::set<std::string>> alias_groups;
        for (auto &constraint : constraints)
        {
            alias_groups.insert(constraint.aliases);
        }

        return alias_groups;
    }
};
