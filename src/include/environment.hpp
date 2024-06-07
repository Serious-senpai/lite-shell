#pragma once

#include "format.hpp"
#include "standard.hpp"
#include "strip.hpp"
#include "utils.hpp"

namespace liteshell
{
    /**
     * @brief Represent the current environment of the shell.
     *
     * This class mostly contains data about active environment _variables.
     */
    class Environment
    {
    private:
        const boost::regex _var_resolver = boost::regex(R"((?<!\$)\$(?:\{(\w+)\}|(\w+)))");
        const boost::regex _escape_resolver = boost::regex(R"(\$\$)");
        std::map<std::string, std::string> _variables;

        Environment(const Environment &) = delete;
        Environment &operator=(const Environment &) = delete;

    public:
        /**
         * @brief Construct a new `Environment` object
         */
        Environment();

        /**
         * @brief Set a value for an environment variable
         *
         * @param name The name of the variable
         * @param value The value of the variable
         *
         * @return A pointer to the current environment
         */
        Environment *set_value(const std::string &name, const std::string &value);

        /**
         * @brief Get the value of an environment variable
         *
         * @param name The name of the variable
         * @return The value of the variable, or an empty string if not found
         */
        std::string get_value(const std::string &name) const;

        /**
         * @brief Get a mapping from environment _variables to their values
         *
         * @return A mapping from environment _variables to their values
         */
        std::map<std::string, std::string> get_values() const;

        /**
         * @brief Resolve all environment _variables in a message
         *
         * @param message The message to resolve
         * @return The resolved message
         */
        std::string resolve(const std::string &message) const;

        /**
         * @brief Evaluate a mathematical expression
         *
         * @param expression The expression to evaluate
         * @return The result of the evaluation
         */
        long long eval_ll(const std::string &expression) const;
    };
}
