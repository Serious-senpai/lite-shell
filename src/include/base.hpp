#pragma once

#include "constraint.hpp"
#include "context.hpp"
#include "standard.hpp"

/*
Abstract base class for all local commands of the shell.

All built-in commands must inherit this class.
*/
class BaseCommand
{
private:
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
    const ArgumentsConstraint constraint;

    BaseCommand(
        const std::string &name,
        const std::string &description,
        const std::string &long_description,
        const std::initializer_list<std::string> &aliases,
        const ArgumentsConstraint &constraint)
        : name(name),
          description(description),
          long_description(long_description),
          aliases(aliases),
          constraint(constraint) {}

    virtual ~BaseCommand() {}

    /*
    Invoke this command and return a new errorlevel for the current shell. Subclasses must implement this method.

    @param context The context in which the command is being invoked under.
    @return The new errorlevel for the shell.
    */
    virtual DWORD run(const Context &context) = 0;

    virtual std::string syntax() const final
    {
        std::string result = name + " ";
        if (constraint.require_context_parsing && constraint.arguments_checking)
        {
            if (constraint.args_bounds.first == constraint.args_bounds.second)
            {
                result += format("<%d argument(s)>", constraint.args_bounds.first - 1);
            }
            else
            {
                result += format("<%d-%d argument(s)>", constraint.args_bounds.first - 1, constraint.args_bounds.second - 1);
            }

            for (auto &aliases : constraint.get_alias_groups())
            {
                result += " <";

                if (aliases.empty())
                {
                    throw std::runtime_error("Alias group is unexpectedly empty");
                }

                std::vector<std::string> aliases_vector(aliases.begin(), aliases.end());
                for (unsigned i = 0; i < aliases_vector.size() - 1; i++)
                {
                    result += aliases_vector[i] + "|";
                }
                result += aliases_vector.back() + "> <values...>";
            }
        }
        else
        {
            result += "<...>";
        }

        return result;
    }
};