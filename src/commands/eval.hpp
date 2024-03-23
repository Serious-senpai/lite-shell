#pragma once

#include <base.hpp>
#include <client.hpp>

ArgumentsConstraint __constraint_EvalCommand()
{
    ArgumentsConstraint constraint(2, 2);
    constraint.add_argument("-m", "treat the input as a mathematical expression instead of a string and evaluate it", 0, 1);
    constraint.add_argument("-p", "print the positional argument to stdout, read stdin and treat it as the input", 0, 1);
    constraint.add_argument("-s", "save the input to an environment variable instead of printing to stdout", 0, 2);
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
        if (context.kwargs.count("-p"))
        {
            std::cout << input;
            std::cout.flush();

            input = context.client->stream.getline(true);
        }

        auto result = context.kwargs.count("-m") ? std::to_string(context.client->get_environment()->eval_ll(input)) : input;
        auto iter = context.kwargs.find("-s");
        if (iter == context.kwargs.end())
        {
            std::cout << result << std::endl;
        }
        else
        {
            if (iter->second.size() < 2)
            {
                throw std::invalid_argument("Missing environment variable name");
            }

            context.client->get_environment()->set_variable(iter->second.back(), result);
        }

        return 0;
    }
};
