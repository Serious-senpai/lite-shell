#pragma once

#include <all.hpp>

CommandConstraint __constraint_EvalCommand()
{
    CommandConstraint constraint(2, 2);
    constraint.add_argument("-m", false, "treat the input as a mathematical expression instead of a string and evaluate it", 0, 0);
    constraint.add_argument("-p", false, "print the positional argument to stdout, read stdin and treat it as the input", 0, 0);
    constraint.add_argument("-s", false, "save the input to an environment variable instead of printing to stdout", 1, 1);
    return constraint;
}

class EvalCommand : public BaseCommand
{
public:
    EvalCommand()
        : BaseCommand(
              "eval",
              "Evaluate an expression",
              "",
              {},
              __constraint_EvalCommand()) {}

    DWORD run(const Context &context)
    {
        auto input = context.args[1];
        if (context.present.count("-p"))
        {
            input = context.client->stream.getline(input, InputStream::FORCE_STDOUT | InputStream::FORCE_STDIN);
        }

        auto result = context.present.count("-m") ? std::to_string(context.client->get_environment()->eval_ll(input)) : input;
        if (!context.present.count("-s"))
        {
            std::cout << result << std::endl;
        }
        else
        {
            context.client->get_environment()->set_value(context.kwargs.at("-s")[0], result);
        }

        return 0;
    }
};
