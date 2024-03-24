#pragma once

#include <base.hpp>
#include <client.hpp>

CommandConstraint __constraint_EvalCommand()
{
    CommandConstraint constraint(2, 2);
    constraint.add_argument(false, "treat the input as a mathematical expression instead of a string and evaluate it", 0, 0, "-m");
    constraint.add_argument(false, "print the positional argument to stdout, read stdin and treat it as the input", 0, 0, "-p");
    constraint.add_argument(false, "save the input to an environment variable instead of printing to stdout", 1, 1, "-s");
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
            std::cout << input;
            std::cout.flush();

            input = context.client->stream.getline(true);
        }

        auto result = context.present.count("-m") ? std::to_string(context.client->get_environment()->eval_ll(input)) : input;
        if (!context.present.count("-s"))
        {
            std::cout << result << std::endl;
        }
        else
        {
            context.client->get_environment()->set_variable(context.kwargs.at("-s")[0], result);
        }

        return 0;
    }
};
