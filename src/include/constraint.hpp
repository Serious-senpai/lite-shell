#pragma once

#include "format.hpp"
#include "utils.hpp"

namespace liteshell
{
    class _BaseArgument
    {
    public:
        /**
         * @brief Whether this argument is required or optional
         */
        const bool required;

        _BaseArgument(const bool required) : required(required) {}

        /**
         * @brief A string displaying this argument in the "Usage" section of the help menu
         */
        virtual std::string display() const = 0;
    };

    /**
     * @brief Represents a command-line positional argument
     */
    class PositionalArgument : public _BaseArgument
    {
    public:
        /**
         * @brief The name of the positional argument
         */
        const std::string name;

        /**
         * @brief The string describing this positional argument
         */
        const std::string help;

        /**
         * @brief Whether this positional argument can have multiple values
         */
        const bool many;

        /**
         * @brief Construct a new `PositionalArgument`
         *
         * @param name The name of the positional argument
         * @param help The string describing this positional argument
         * @param required Whether this positional argument is required or optional
         * @param many Whether this positional argument can have multiple values
         */
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

        /** @copydoc _BaseArgument::display */
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
        const std::map<std::string, std::size_t> _positional_map;
        static std::map<std::string, std::size_t> _create_map(const std::vector<PositionalArgument> &positional);

    public:
        /**
         * @brief An array of positional arguments within this constraint
         */
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
                for (std::size_t i = 0; i < positional.size() - 1; i++)
                {
                    if (positional[i].many)
                    {
                        throw std::invalid_argument(utils::format("Variadic positional argument must be the last argument (\"%s\")", positional[i].name.c_str()));
                    }
                }
            }
        }

        /**
         * @brief Find a `PositionalArgument` object with the given name.
         *
         * Get a `PositionalArgument` object with the given name. If no positional argument was found,
         * an `std::invalid_argument` exception is thrown.
         *
         * @param name The name of the positional argument
         * @return The `PositionalArgument` object with the given name
         * @throw `std::invalid_argument` if the positional argument does not exist
         */
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

    std::map<std::string, std::size_t> _SupportsMultiplePositionalArguments::_create_map(
        const std::vector<PositionalArgument> &positional)
    {
        std::map<std::string, std::size_t> result;
        for (std::size_t i = 0; i < positional.size(); i++)
        {
            result.insert(std::make_pair(positional[i].name, i));
        }

        return result;
    }

    /**
     * @brief Represents a command-line option
     */
    class Option : public _BaseArgument, _SupportsMultiplePositionalArguments
    {
    public:
        /**
         * @brief The short name of the option e.g. `-v`
         */
        const std::optional<std::string> short_name;

        /**
         * @brief The long name of the option e.g. `--verbose`
         */
        const std::optional<std::string> long_name;

        /**
         * @brief The string describing this option
         */
        const std::string help;
        using _SupportsMultiplePositionalArguments::positional;

        /**
         * @brief Construct a new `Option`
         *
         * @param short_name The short name of the option e.g. `-v`
         * @param long_name The long name of the option e.g. `--verbose`
         * @param help The string describing this option
         * @param positional The positional arguments for this option
         * @param required Whether this option is required or optional
         */
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
                    throw std::invalid_argument(utils::format("\"%s\" is not a valid option short name", short_name->c_str()));
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
                    throw std::invalid_argument(utils::format("\"%s\" is not a valid option long name", long_name->c_str()));
                }
            }
        }

        /**
         * @brief Get all names of this option
         *
         * @return A vector containing all names of this option (length 1 or 2)
         */
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

        /** @copydoc _BaseArgument::display */
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

    /** @brief Represents the constraints for a command */
    class CommandConstraint : public _SupportsMultiplePositionalArguments
    {
    private:
        std::vector<Option> options;
        std::map<std::string, std::size_t> options_map;

        void check_duplicate_option_name(const std::string &name) const
        {
            if (has_option(name))
            {
                throw std::invalid_argument(utils::format("Argument \"%s\" already exists", name.c_str()));
            }
        }

    public:
        /** @brief Construct a `CommandConstraint` with no positional argument */
        CommandConstraint() : _SupportsMultiplePositionalArguments({}) {}

        /**
         * @brief Construct a `CommandConstraint` with the specified positional arguments
         *
         * @param positional The positional arguments for this command
         */
        CommandConstraint(const std::vector<PositionalArgument> &positional)
            : _SupportsMultiplePositionalArguments(positional) {}

        /**
         * @brief Construct a `CommandConstraint` with 1 positional argument
         *
         * @param name The name of the positional argument
         * @param help The help message for the positional argument
         * @param required Whether the positional argument is required
         * @param many Whether the positional argument can have multiple values
         */
        CommandConstraint(
            const std::string &name, const std::string &help, const bool required,
            const bool many = false)
            : CommandConstraint({PositionalArgument(name, help, many, required)}) {}

        /**
         * @brief Construct a `CommandConstraint` with 2 positional arguments
         *
         * @param name_1 The name of the first positional argument
         * @param help_1 The help message for the first positional argument
         * @param required_1 Whether the first positional argument is required
         * @param name_2 The name of the second positional argument
         * @param help_2 The help message for the second positional argument
         * @param required_2 Whether the second positional argument is required
         * @param many Whether the second positional argument can have multiple values
         */
        CommandConstraint(
            const std::string &name_1, const std::string &help_1, const bool required_1,
            const std::string &name_2, const std::string &help_2, const bool required_2,
            const bool many = false)
            : CommandConstraint(
                  {PositionalArgument(name_1, help_1, false, required_1),
                   PositionalArgument(name_2, help_2, many, required_2)}) {}

        /**
         * @brief Construct a `CommandConstraint` with 3 positional arguments
         *
         * @param name_1 The name of the first positional argument
         * @param help_1 The help message for the first positional argument
         * @param required_1 Whether the first positional argument is required
         * @param name_2 The name of the second positional argument
         * @param help_2 The help message for the second positional argument
         * @param required_2 Whether the second positional argument is required
         * @param name_3 The name of the third positional argument
         * @param help_3 The help message for the third positional argument
         * @param required_3 Whether the third positional argument is required
         * @param many Whether the third positional argument can have multiple values
         */
        CommandConstraint(
            const std::string &name_1, const std::string &help_1, const bool required_1,
            const std::string &name_2, const std::string &help_2, const bool required_2,
            const std::string &name_3, const std::string &help_3, const bool required_3,
            const bool many = false)
            : CommandConstraint(
                  {PositionalArgument(name_1, help_1, false, required_1),
                   PositionalArgument(name_2, help_2, false, required_2),
                   PositionalArgument(name_3, help_3, many, required_3)}) {}

        /** @brief Whether there exists an `Option` with the given name */
        bool has_option(const std::string &name) const
        {
            return options_map.find(name) != options_map.end();
        }

        /** @brief Get a mapping from option names to their corresponding options */
        std::map<std::string, Option> get_options_map() const
        {
            std::map<std::string, Option> result;
            for (auto &option : options)
            {
                for (auto &name : option.names())
                {
                    result.insert(std::make_pair(name, option));
                }
            }

            return result;
        }

        /** @brief Get a vector of all options with no duplication */
        std::vector<Option> get_options_vector() const
        {
            return options;
        }

        /**
         * @brief Add an `Option` to this constraint
         *
         * @param option The `Option` to add
         * @return A pointer to this `CommandConstraint` object
         */
        CommandConstraint *add_option(const Option &option)
        {
            options.push_back(option);
            for (auto &name : option.names())
            {
                check_duplicate_option_name(name);
                options_map[name] = options.size() - 1;
            }

            return this;
        }

        /**
         * @brief Add an `Option` to this constraint with a list of inner positional arguments
         *
         * @param short_name The short name of the option
         * @param long_name The long name of the option
         * @param help The help message for the option
         * @param arguments The positional arguments for the option
         * @param required Whether the option is required
         *
         * @return A pointer to this `CommandConstraint` object
         */
        CommandConstraint *add_option(
            const std::string &short_name,
            const std::string &long_name,
            const std::string &help,
            const std::vector<PositionalArgument> &arguments,
            const bool required = false)
        {
            return add_option(Option(short_name, long_name, help, arguments, required));
        }

        /**
         * @brief Add an `Option` to this constraint with only 1 inner positional argument
         *
         * @param short_name The short name of the option
         * @param long_name The long name of the option
         * @param help The help message for the option
         * @param argument The only positional argument for the option
         * @param required Whether the option is required
         *
         * @return A pointer to this `CommandConstraint` object
         */
        CommandConstraint *add_option(
            const std::string &short_name,
            const std::string &long_name,
            const std::string &help,
            const PositionalArgument &argument,
            const bool required = false)
        {
            return add_option(Option(short_name, long_name, help, {argument}, required));
        }

        /**
         * @brief Add an `Option` to this constraint with a list of inner positional arguments
         *
         * @param name The name of the option, the method will auto-detect whether this is a short or long name
         * @param help The help message for the option
         * @param arguments The positional arguments for the option
         * @param required Whether the option is required
         *
         * @return A pointer to this `CommandConstraint` object
         */
        CommandConstraint *add_option(
            const std::string &name,
            const std::string &help,
            const std::vector<PositionalArgument> &arguments,
            const bool required = false)
        {
            if (utils::is_valid_long_option(name))
            {
                return add_option(Option(std::nullopt, name, help, arguments, required));
            }

            if (utils::is_valid_short_option(name))
            {
                return add_option(Option(name, std::nullopt, help, arguments, required));
            }

            throw std::invalid_argument(utils::format("\"%s\" is neither a valid short or long option name", name.c_str()));
        }

        /**
         * @brief Add an `Option` to this constraint with only 1 inner positional argument
         *
         * @param name The name of the option, the method will auto-detect whether this is a short or long name
         * @param help The help message for the option
         * @param argument The only positional argument for the option
         * @param required Whether the option is required
         *
         * @return A pointer to this `CommandConstraint` object
         */
        CommandConstraint *add_option(
            const std::string &name,
            const std::string &help,
            const PositionalArgument &argument,
            const bool required = false)
        {
            std::vector<PositionalArgument> arguments(1, argument);
            return add_option(name, help, arguments, required);
        }

        /**
         * @brief Add an `Option` to this constraint with no inner positional argument
         *
         * @param name The name of the option, the method will auto-detect whether this is a short or long name
         * @param help The help message for the option
         * @param required Whether the option is required
         *
         * @return A pointer to this `CommandConstraint` object
         */
        CommandConstraint *add_option(
            const std::string &name,
            const std::string &help,
            const bool required = false)
        {
            return add_option(name, help, {}, required);
        }
    };
}
