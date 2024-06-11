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
         * @brief Constructs a command object and initializes its properties.
         *
         * @param name specifies the command's identifier.
         * @param description provides a concise explanation of the command's purpose.
         * @param long_description elaborates on the command's functionality and usage.
         * @param aliases lists alternative identifiers by which the command may be executed.
         * @param constraint defines the permissible argument configurations for the command.
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
         * @brief instantiates a command object with specified attributes, excluding aliases.
         *
         * @param name identifies the command's unique name.
         * @param description summarizes the command's purpose briefly.
         * @param long_description details the command's functionality and guidance for use.
         * @param constraint specifies the command's argument requirements and limitations.
         */
        BaseCommand(
            const std::string &name,
            const std::string &description,
            const std::string &long_description,
            const CommandConstraint &constraint)
            : BaseCommand(name, description, long_description, {}, constraint)
        {
        }

        /**
         * @brief Executes the command and updates the shell's error level.
         *
         * This method must be overridden by subclasses to define specific command behavior.
         *
         * @param context represents the environment in which the command is executed.
         * @return The updated error level for the shell after command execution.
         */
        virtual DWORD run(const Context &context) = 0;

        /**
         * @brief Produces the command's help documentation.
         *
         * Generates a string that encapsulates the command's help details.
         * This includes a description of the command, instructions for use, associated aliases, and argument specifications.
         *
         * @return A comprehensive string detailing the help information.
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
                    stream << " " << argument.display() << std::endl;
                    stream << "  " << argument.help << std::endl;
                }

                for (auto &argument : constraint.get_options_vector())
                {
                    auto names = argument.names();
                    stream << " " << utils::join(names.begin(), names.end(), " | ") << std::endl;
                    stream << "  " << argument.help << std::endl;

                    for (auto &p : argument.positional)
                    {
                        stream << "   " << p.display() << std::endl;
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
