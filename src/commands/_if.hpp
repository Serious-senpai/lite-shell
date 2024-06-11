#pragma once

#include <all.hpp>

class _IfCommand : public liteshell::BaseCommand
{
public:
    _IfCommand()
        : liteshell::BaseCommand(
              "_if",
              "Hidden command",
              "",
              liteshell::CommandConstraint(
                  "x", "", true,
                  "operator", "", true,
                  "y", "", true,
                  "true", "", true,
                  "false", "", true)
                  .add_option("-m", "", false))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        auto first = context.get("x"), op = context.get("operator"), second = context.get("y");
        bool result;
        if (context.present.count("-m"))
        {
            auto f = context.client->get_environment()->eval_ll(first),
                 s = context.client->get_environment()->eval_ll(second);
            if (op == "==")
            {
                result = f == s;
            }
            else if (op == "!=")
            {
                result = f != s;
            }
            else if (op == "<")
            {
                result = f < s;
            }
            else if (op == ">")
            {
                result = f > s;
            }
            else if (op == "<=")
            {
                result = f <= s;
            }
            else if (op == ">=")
            {
                result = f >= s;
            }
            else
            {
                throw std::invalid_argument("Invalid operator");
            }
        }
        else
        {
            if (op == "==")
            {
                result = first == second;
            }
            else if (op == "!=")
            {
                result = first != second;
            }
            else if (op == "<")
            {
                result = first < second;
            }
            else if (op == ">")
            {
                result = first > second;
            }
            else if (op == "<=")
            {
                result = first <= second;
            }
            else if (op == ">=")
            {
                result = first >= second;
            }
            else
            {
                throw std::invalid_argument("Invalid operator");
            }
        }

        context.client->get_stream()->jump(context.get(result ? "true" : "false"));

        return 0;
    }
};
