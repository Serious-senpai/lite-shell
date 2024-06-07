#include "base.hpp"

namespace liteshell
{
    BaseCommand::BaseCommand(
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

    BaseCommand::BaseCommand(
        const std::string &name,
        const std::string &description,
        const std::string &long_description,
        const CommandConstraint &constraint)
        : BaseCommand(name, description, long_description, {}, constraint)
    {
    }

    std::string BaseCommand::help() const
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
}
