#pragma once

#include <all.hpp>

class ArrayCommand : public liteshell::BaseCommand
{
public:
    ArrayCommand()
        : liteshell::BaseCommand(
              "array",
              "Store an array of environment variables",
              "The resulting variables will be the base variable name concatenated with a suffix \"_i\", where i "
              "is the index of that tokens (starting from 0).\n"
              "The base variable will have it value set to the number of elements.\n"
              "For example, \"array a 1 2 abc x y z\" gives a_0 = 1, a_1 = 2, a_2 = abc and so on.",
              liteshell::CommandConstraint(
                  "var", "Base variable name", true,
                  "tokens", "The tokens to store", true, true))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        const auto base_name = context.get("var");
        const auto tokens = context.values.at("tokens");

        const auto environment_ptr = context.client->get_environment();
        environment_ptr->set_value(base_name, std::to_string(tokens.size()));
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
            environment_ptr->set_value(utils::format("%s_%u", base_name.c_str(), i), tokens[i]);
        }

        return 0;
    }
};
