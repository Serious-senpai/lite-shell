#pragma once

#include <all.hpp>

liteshell::CommandConstraint __constraint_EvalCommand()
{
    liteshell::CommandConstraint constraint("expression", "A string expression, or a math expression if -m is specified", true);
    constraint.add_option("-m", "Treat the input as a mathematical expression instead of a string and evaluate it");
    constraint.add_option("-p", "Print the input to stdout, read stdin and treat it as the original input");
    constraint.add_option(
        "-s",
        "Save the input to an environment variable instead of printing to stdout",
        liteshell::PositionalArgument("var", "The variable name", false, true));

    return constraint;
}

class EvalCommand : public liteshell::BaseCommand
{
public:
    EvalCommand()
        : liteshell::BaseCommand(
              "eval",
              "Evaluate an expression",
              "The default behavior of this command is to treat the argument as a string and print it to stdout (which is\n"
              "similar to the \"echoln\" command. Different behavior can be achieved by using the parameters listed below.",
              {},
              __constraint_EvalCommand())
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        auto input = context.get("expression");
        if (context.present.count("-p"))
        {
            input = context.client->get_stream()->getline(input, liteshell::InputStream::FORCE_STDOUT | liteshell::InputStream::FORCE_STDIN);
        }

        auto result = context.present.count("-m") ? std::to_string(context.client->get_environment()->eval_ll(input)) : input;
        if (context.present.count("-s"))
        {
            auto name = context.get("-s var");
            if (!utils::is_valid_variable(name))
            {
                throw std::invalid_argument(utils::format("Invalid variable name \"%s\"", name.c_str()));
            }

            context.client->get_environment()->set_value(name, result);
        }
        else
        {
            std::cout << result << std::endl;
        }

        return 0;
    }
};
