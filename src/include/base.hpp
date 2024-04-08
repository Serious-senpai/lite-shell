#pragma once

#include "constraint.hpp"
#include "context.hpp"

/*
Abstract base class for all local commands of the shell.

All built-in commands must inherit this class.
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
          constraint(constraint) {}

    virtual ~BaseCommand() {}

    /**
    Invoke this command and return a new errorlevel for the current shell. Subclasses must implement this method.

    @param context The context in which the command is being invoked under.
    @return The new errorlevel for the shell.
    */
    virtual DWORD run(const Context &context) = 0;

    /**
    Returns a string containing the help information for this command.
    The help information includes the command description, usage, aliases, and parameters.

    @return A string containing the help information.
    */
    virtual std::string help() const final
    {
        std::stringstream stream;
        stream << description << std::endl;
        stream << "Usage: " << std::endl;

        const auto groups = constraint.get_alias_groups();
        if (constraint.require_context_parsing && constraint.arguments_checking)
        {
            std::vector<std::set<std::string>> optional, required;
            for (auto &aliases : groups)
            {
                if (aliases.empty())
                {
                    throw std::runtime_error(format("Command %s has an unexpected empty alias group", name.c_str()));
                }

                auto name = *aliases.begin();
                auto c = constraint.get_constraint(name);
                if (c.required)
                {
                    required.push_back(aliases);
                }
                else
                {
                    optional.push_back(aliases);
                }
            }

            if (optional.size() > 64) // unsigned long long has 64 bits
            {
                throw std::runtime_error(format("Command %s has %d optional arguments, only a maximum of 64 is supported", name.c_str(), optional.size()));
            }

            std::string args_display;
            auto bounds = constraint.args_bounds;
            bounds.first--;
            bounds.second--;
            if (bounds.first == bounds.second)
            {
                args_display = format("[%d %s]", bounds.second, ngettext(bounds.second == 1, "argument", "arguments").c_str());
            }
            else
            {
                args_display = format("[%d-%d %s]", bounds.first, bounds.second, ngettext(bounds.second == 1, "argument", "arguments").c_str());
            }

            auto display = [this](const std::set<std::string> aliases)
            {
                std::stringstream display;
                display << join(aliases.begin(), aliases.end(), "|");

                auto bounds = constraint.get_constraint(*aliases.begin()).bounds;
                if (bounds.second > 0)
                {
                    if (bounds.first == bounds.second)
                    {
                        display << " [" << bounds.second << " " << ngettext(bounds.second == 1, "argument", "arguments") << "]";
                    }
                    else
                    {
                        display << " [" << bounds.first << "-" << bounds.second << " " << ngettext(bounds.second == 1, "argument", "arguments") << "]";
                    }
                }

                return display.str();
            };

            for (unsigned long long bitmask = 0ull; bitmask < (1ull << optional.size()); bitmask++)
            {
                stream << "  " << name << " " << args_display;
                for (auto &aliases : required)
                {
                    stream << " " << display(aliases);
                }

                for (unsigned long long i = 0; i < optional.size(); i++)
                {
                    if (bitmask & (1ull << i))
                    {
                        stream << " " << display(optional[i]);
                    }
                }
                stream << std::endl;
            }
        }
        else
        {
            stream << "  " << name << " [...]" << std::endl;
        }

        stream << long_description << std::endl;
        if (!aliases.empty())
        {
            stream << "Aliases: " << join(aliases.begin(), aliases.end(), ", ") << std::endl;
        }

        if (!groups.empty())
        {
            stream << "Parameters:" << std::endl;
            for (auto &aliases : groups)
            {
                auto c = constraint.get_constraint(*aliases.begin());
                stream << " " << join(aliases.begin(), aliases.end(), "|") << " " << ngettext(c.required, "(required)", "(optional)") << std::endl;
                stream << "  " << c.help << std::endl;
            }
        }

        return stream.str();
    }
};