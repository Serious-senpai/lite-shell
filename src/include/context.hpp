#pragma once

#include "constraint.hpp"
#include "error.hpp"
#include "split.hpp"

namespace liteshell
{
    /**
     * @brief The command invocation context.
     *
     * Represents the context in which a command is being invoked under.
     *
     * This class contains a lot of metadata to help you understand more about the invocation context. Instances
     * of this class are not created manually and instead passed around to commands as the only parameter.
     */
    class Context
    {
    private:
        Context(
            const std::string &message,
            const std::vector<std::string> &tokens,
            const std::map<std::string, std::vector<std::string>> &values,
            const std::set<std::string> &present,
            const std::shared_ptr<class Client> &client,
            const std::optional<CommandConstraint> &constraint);

    public:
        /** @brief A suffix indicating that a command message should be run in a background */
        static const char BACKGROUND_SUFFIX = '%';

        /** @brief The message that triggered the command being executed. */
        const std::string message;

        /** @brief The list of tokens after parsing the message: e.g. `args a b -c d` will give `[args, a, b, -c, d]`. */
        const std::vector<std::string> tokens;

        /** @brief A mapping of argument names to their values. */
        const std::map<std::string, std::vector<std::string>> values;

        /** @brief The options presenting in the command. */
        const std::set<std::string> present;

        /** @brief A pointer to the client that contains the command being executed. */
        const std::shared_ptr<class Client> client;

        /** @brief The arguments constraint of this context object */
        const std::optional<CommandConstraint> constraint;

        /**
         * @brief Get the first value of an argument.
         *
         * Roughly equivalent to `.values[name][0]`.
         * @see `values`
         *
         * @param name The name of the argument to get
         * @return The first value of the argument
         */
        std::string get(const std::string &name) const;

        /**
         * @brief Parse this context with another constraint.
         *
         * @param constraint The new constraint to parse the context with
         * @return A new context with the new constraint applied
         */
        Context parse(const std::optional<CommandConstraint> &constraint) const;

        /**
         * @brief Replace the first token of the original command message
         *
         * @param token A new token to replace with
         * @return A new Context with the first argument replaced with the given token
         */
        Context replace_call(const std::string &token) const;

        /**
         * @brief Remove the background suffix token from the command message.
         * @see `BACKGROUND_SUFFIX`
         *
         * @return A new context with the background suffix token removed
         */
        Context strip_background_request() const;

        /**
         * @brief Determine whether this context is requesting to run in a background process.
         * @see `BACKGROUND_SUFFIX`
         *
         * @return `true` if the context is requesting to run in the background, `false` otherwise
         */
        bool is_background_request() const;

        /**
         * @brief Construct a `Context` from a message
         *
         * @param client A pointer to the Client object
         * @param message The message to construct the context from
         * @param constraint The constraint to parse the context with (if `std::nullopt` is provided, the
         * resulting `Context` will have its `Context::values` and `Context::present` be empty containers)
         * @return A new context object
         */
        static Context get_context(const std::shared_ptr<Client> &client, const std::string &message, const std::optional<CommandConstraint> &constraint = std::nullopt);
    };
}
