#pragma once

#include "format.hpp"
#include "utils.hpp"

namespace liteshell
{
    class _BaseArgument
    {
    public:
        const bool required;

        _BaseArgument(const bool required) : required(required) {}

        virtual std::string display() const = 0;
    };

    class PositionalArgument : public _BaseArgument
    {
    public:
        const std::string name;
        const std::string help;
        const bool many;

        PositionalArgument(
            const std::string &name,
            const std::string &help,
            const bool many,
            const bool required)
            : _BaseArgument(required), name(name), help(help), many(many)
        {
            if (!utils::is_valid_variable(name))
            {
                throw std::invalid_argument(utils::format("\"%s\" is not a valid argument name", name.c_str()));
            }
        }

        std::string display() const
        {
            auto result = "<" + name + ">";
            if (many)
            {
                result += "...";
            }

            if (!required)
            {
                result = "[" + result + "]";
            }

            return result;
        }
    };

    class _SupportsMultiplePositionalArguments
    {
    private:
        const std::map<std::string, unsigned> _positional_map;
        static std::map<std::string, unsigned> _create_map(const std::vector<PositionalArgument> &positional);

    public:
        const std::vector<PositionalArgument> positional;

        _SupportsMultiplePositionalArguments(const std::vector<PositionalArgument> &positional)
            : _positional_map(_create_map(positional)), positional(positional)
        {
            std::set<std::string> names;
            for (auto &argument : positional)
            {
                if (names.count(argument.name))
                {
                    throw std::invalid_argument(utils::format("Duplicate positional argument \"%s\" within an Option", argument.name.c_str()));
                }

                names.insert(argument.name);
            }

            if (!positional.empty())
            {
                for (unsigned i = 0; i < positional.size() - 1; i++)
                {
                    if (positional[i].many)
                    {
                        throw std::invalid_argument(utils::format("Variadic positional argument must be the last argument (\"%s\")", positional[i].name.c_str()));
                    }
                }
            }
        }

        PositionalArgument get_positional(const std::string &name) const
        {
            auto iter = _positional_map.find(name);
            if (iter == _positional_map.end())
            {
                throw std::invalid_argument(utils::format("Positional argument \"%s\" does not exist", name.c_str()));
            }

            return positional[iter->second];
        }
    };

    std::map<std::string, unsigned> _SupportsMultiplePositionalArguments::_create_map(
        const std::vector<PositionalArgument> &positional)
    {
        std::map<std::string, unsigned> result;
        for (unsigned i = 0; i < positional.size(); i++)
        {
            result.insert(std::make_pair(positional[i].name, i));
        }

        return result;
    }

    class Option : public _BaseArgument, _SupportsMultiplePositionalArguments
    {
    public:
        const std::optional<std::string> short_name;
        const std::optional<std::string> long_name;
        const std::string help;
        using _SupportsMultiplePositionalArguments::positional;

        Option(
            const std::optional<std::string> &short_name,
            const std::optional<std::string> &long_name,
            const std::string &help,
            const std::vector<PositionalArgument> &positional,
            const bool required) : _BaseArgument(required),
                                   _SupportsMultiplePositionalArguments(positional),
                                   short_name(short_name),
                                   long_name(long_name),
                                   help(help)
        {
            if (!short_name.has_value() && !long_name.has_value())
            {
                throw std::runtime_error("Option cannot have both short_name and long_name set to std::nullopt");
            }

            if (short_name.has_value())
            {
                if (!utils::startswith(*short_name, "-"))
                {
                    throw std::invalid_argument(utils::format("Option name must start with \"-\" (currently \"%s\")", short_name->c_str()));
                }
                if (!utils::is_valid_short_option(*short_name))
                {
                    throw std::invalid_argument(utils::format("\"%s\" is not a valid argument name", short_name->c_str()));
                }
            }

            if (long_name.has_value())
            {
                if (!utils::startswith(*long_name, "--"))
                {
                    throw std::invalid_argument(utils::format("Option name must start with \"--\" (currently \"%s\")", long_name->c_str()));
                }
                if (!utils::is_valid_long_option(*long_name))
                {
                    throw std::invalid_argument(utils::format("\"%s\" is not a valid argument name", long_name->c_str()));
                }
            }
        }

        std::vector<std::string> names() const
        {
            std::vector<std::string> result;
            if (short_name.has_value())
            {
                result.push_back(*short_name);
            }

            if (long_name.has_value())
            {
                result.push_back(*long_name);
            }

            return result;
        }

        std::string display() const
        {
            std::stringstream stream;
            stream << (short_name.has_value() ? *short_name : *long_name);

            for (auto &argument : positional)
            {
                stream << " " << argument.display();
            }

            auto result = stream.str();
            if (!required)
            {
                result = "[" + result + "]";
            }

            return result;
        }
    };

    /* Represents the constraints for a command */
    class CommandConstraint : public _SupportsMultiplePositionalArguments
    {
    private:
        std::vector<Option> options;
        std::map<std::string, unsigned> options_map;

        void check_valid_option_name(const std::string &name) const
        {
            if (has_option(name))
            {
                throw std::invalid_argument(utils::format("Argument \"%s\" already exists", name.c_str()));
            }
        }

    public:
        CommandConstraint() : _SupportsMultiplePositionalArguments({}) {}

        CommandConstraint(const std::vector<PositionalArgument> &positional)
            : _SupportsMultiplePositionalArguments(positional) {}

        CommandConstraint(
            const std::string &name, const std::string &help, const bool required,
            const bool many = false)
            : CommandConstraint({PositionalArgument(name, help, many, required)}) {}

        CommandConstraint(
            const std::string &name_1, const std::string &help_1, const bool required_1,
            const std::string &name_2, const std::string &help_2, const bool required_2,
            const bool many = false)
            : CommandConstraint(
                  {PositionalArgument(name_1, help_1, false, required_1),
                   PositionalArgument(name_2, help_2, many, required_2)}) {}

        CommandConstraint(
            const std::string &name_1, const std::string &help_1, const bool required_1,
            const std::string &name_2, const std::string &help_2, const bool required_2,
            const std::string &name_3, const std::string &help_3, const bool required_3,
            const bool many = false)
            : CommandConstraint(
                  {PositionalArgument(name_1, help_1, false, required_1),
                   PositionalArgument(name_2, help_2, false, required_2),
                   PositionalArgument(name_3, help_3, many, required_3)}) {}

        bool has_option(const std::string &name) const
        {
            return options_map.find(name) != options_map.end();
        }

        std::map<std::string, Option> get_options_map() const
        {
            std::map<std::string, Option> result;
            for (auto &option : options)
            {
                if (option.short_name.has_value())
                {
                    result.insert(std::make_pair(*option.short_name, option));
                }

                if (option.long_name.has_value())
                {
                    result.insert(std::make_pair(*option.long_name, option));
                }
            }

            return result;
        }

        std::vector<Option> get_options_vector() const
        {
            return options;
        }

        CommandConstraint *add_option(const Option &argument)
        {
            options.push_back(argument);

            if (argument.short_name.has_value())
            {
                check_valid_option_name(*argument.short_name);
                options_map[*argument.short_name] = options.size() - 1;
            }

            if (argument.long_name.has_value())
            {
                check_valid_option_name(*argument.long_name);
                options_map[*argument.long_name] = options.size() - 1;
            }

            return this;
        }

        CommandConstraint *add_option(
            const std::string &short_name,
            const std::string &long_name,
            const std::string &help,
            const std::vector<PositionalArgument> &arguments,
            const bool required = false)
        {
            return add_option(Option(short_name, long_name, help, arguments, required));
        }

        CommandConstraint *add_option(
            const std::string &short_name,
            const std::string &long_name,
            const std::string &help,
            const PositionalArgument &argument,
            const bool required = false)
        {
            return add_option(Option(short_name, long_name, help, {argument}, required));
        }

        CommandConstraint *add_option(
            const std::string &name,
            const std::string &help,
            const std::vector<PositionalArgument> &arguments,
            const bool required = false)
        {
            if (utils::startswith(name, "--"))
            {
                return add_option(Option(std::nullopt, name, help, arguments, required));
            }

            return add_option(Option(name, std::nullopt, help, arguments, required));
        }

        CommandConstraint *add_option(
            const std::string &name,
            const std::string &help,
            const PositionalArgument &argument,
            const bool required = false)
        {
            std::vector<PositionalArgument> arguments(1, argument);
            return add_option(name, help, arguments, required);
        }

        CommandConstraint *add_option(
            const std::string &name,
            const std::string &help,
            const bool required = false)
        {
            return add_option(name, help, {}, required);
        }
    };
}
