#pragma once

#include "constraint.hpp"
#include "context.hpp"

namespace liteshell
{
    /**
     * @brief Abstract base class for all local commands of the shell.
     *
     * All built-in commands must inherit this class.
     */
    class BaseCommand
    {
    public:
        /** @brief The name of the command */
        const std::string name;

        /** @brief A short description of the command that will be shown when running `help` */
        const std::string description;

        /** @brief A long description of the command that will be shown when running `help <name>` */
        const std::string long_description;

        /** @brief A list of aliases the command can be invoked under. */
        const std::vector<std::string> aliases;

        /** @brief The arguments constraint of the command */
        const CommandConstraint constraint;

        /**
         * @brief Construct a new command and initialize its attributes.
         *
         * @param name The name of the command
         * @param description A short description of the command
         * @param long_description A long description of the command
         * @param aliases A list of aliases the command can be invoked under
         * @param constraint The arguments constraint of the command
         */
        BaseCommand(
            const std::string &name,
            const std::string &description,
            const std::string &long_description,
            const std::initializer_list<std::string> &aliases,
            const CommandConstraint &constraint);

        /**
         * @brief Construct a new command without aliases
         *
         * @param name The name of the command
         * @param description A short description of the command
         * @param long_description A long description of the command
         * @param constraint The arguments constraint of the command
         */
        BaseCommand(
            const std::string &name,
            const std::string &description,
            const std::string &long_description,
            const CommandConstraint &constraint);

        /**
         * @brief Invoke this command and return a new errorlevel for the current shell.
         *
         * Subclasses must implement this method.
         *
         * @param context The context in which the command is being invoked under.
         * @return The new errorlevel for the shell.
         */
        virtual DWORD run(const Context &context) = 0;

        /**
         * @brief Generate the help message.
         *
         * Returns a string containing the help information for this command.
         * The help information includes the command description, usage, aliases, and parameters.
         *
         * @return A string containing the help information.
         */
        virtual std::string help() const final;
    };
}
