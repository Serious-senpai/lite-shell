#pragma once

#include <base.hpp>
#include <client.hpp>

class EvalCommand : public BaseCommand
{
public:
    EvalCommand()
        : BaseCommand(
              "eval",
              "Evaluate an expression",
              "",
              {},
              ArgumentsConstraint(false)) {}

    DWORD run(const Context &context)
    {
        std::cout << context.client->environment->eval_ll(context.get_arguments_string()) << std::endl;
        return 0;
    }
};
