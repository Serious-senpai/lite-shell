#pragma once

#include "constraint.hpp"
#include "context.hpp"

namespace liteshell
{
    /**
     * Abstract base class for all local commands of the shell.
     *
     * All built-in commands must inherit this class.
     */
    class BaseCommand
    {
    public:
        /* The name of the command */
        const std::string name;

        /* A short description of the command that will be shown when running `>help` */
        const std::string description;

        /* A long description of the command that will be shown when running `>help <name>` */
        const std::string long_description;

        /* The list of aliases the command can be invoked under. */
        const std::vector<std::string> aliases;

        /* The arguments constraint for this command */
        const CommandConstraint constraint;

        /* Construct a new command and initialize its attributes */
        BaseCommand(
            const std::string &name,
            const std::string &description,
            const std::string &long_description,
            const std::initializer_list<std::string> &aliases,
            const CommandConstraint &constraint)
            : name(name),
              description(description),
              long_description(long_description),
              aliases(aliases),
              constraint(constraint)
        {
#ifdef DEBUG
            std::cout << "Constructed command \"" << name << "\"" << std::endl;
#endif
        }

        virtual ~BaseCommand() {}

        /**
         *Invoke this command and return a new errorlevel for the current shell. Subclasses must implement this method.
         *
         * @param context The context in which the command is being invoked under.
         * @return The new errorlevel for the shell.
         */
        virtual DWORD run(const Context &context) = 0;

        /**
         * Returns a string containing the help information for this command.
         * The help information includes the command description, usage, aliases, and parameters.
         *
         * @return A string containing the help information.
         */
        virtual std::string help() const final
        {
            std::stringstream stream;
            stream << description << std::endl;
            stream << "Usage: " << std::endl;

            if (!long_description.empty())
            {
                stream << std::endl
                       << long_description << std::endl;
            }

            if (!aliases.empty())
            {
                stream << std::endl
                       << "Aliases: " << utils::join(aliases.begin(), aliases.end(), ", ") << std::endl;
            }

            return stream.str();
        }
    };
}