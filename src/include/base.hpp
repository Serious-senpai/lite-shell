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

        /** @brief The list of aliases the command can be invoked under. */
        const std::vector<std::string> aliases;

        /** @brief The arguments constraint for this command */
        const CommandConstraint constraint;

        /**
         * @brief Construct a new command and initialize its attributes
         *
         * @param name The name of the command
         * @param description A short description of the command
         * @param long_description A long description of the command
         * @param aliases A list of aliases the command can be invoked under
         * @param constraint The arguments constraint for this command
         */
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
            if (!utils::is_valid_command(name))
            {
                throw std::invalid_argument(utils::format("Invalid command name \"%s\"", name.c_str()));
            }

#ifdef DEBUG
            std::cout << "Constructed command \"" << name << "\"" << std::endl;
#endif
        }

        /**
         * @brief The command destructor
         *
         * The default implementation does nothing.
         */
        virtual ~BaseCommand() {}

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
         * @brief Generate the help message
         *
         * Returns a string containing the help information for this command.
         * The help information includes the command description, usage, aliases, and parameters.
         *
         * @return A string containing the help information.
         */
        virtual std::string help() const final
        {
            std::stringstream stream;
            stream << description << std::endl;

            stream << std::endl
                   << "Usage: " << name << " ";

            std::vector<std::string> arguments;
            for (auto &argument : constraint.positional)
            {
                arguments.push_back(argument.display());
            }
            for (auto &argument : constraint.get_options_vector())
            {
                arguments.push_back(argument.display());
            }

            if (!arguments.empty())
            {
                stream << utils::join(arguments.begin(), arguments.end(), " ") << std::endl;
                stream << "Parameters:" << std::endl;

                for (auto &argument : constraint.positional)
                {
                    stream << " " << argument.display() << " " << utils::ngettext(argument.required, "(required)", "(optional)") << std::endl;
                    stream << "  " << argument.help << std::endl;
                }

                for (auto &argument : constraint.get_options_vector())
                {
                    auto names = argument.names();
                    stream << " " << utils::join(names.begin(), names.end(), " | ") << " " << utils::ngettext(argument.required, "(required)", "(optional)") << std::endl;
                    stream << "  " << argument.help << std::endl;

                    for (auto &p : argument.positional)
                    {
                        stream << "   " << p.display() << " " << utils::ngettext(p.required, "(required)", "(optional)") << std::endl;
                        stream << "    " << p.help << std::endl;
                    }
                }
            }

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