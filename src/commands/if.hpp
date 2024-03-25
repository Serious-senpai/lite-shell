#pragma once

#include <base.hpp>
#include <client.hpp>

const char __if_description[] = R"(
The command syntax is: if <value> <operator> <value>
where <operator> must be one of the values: "==", "!=", "<", ">", "<=", ">=".

The strings are compared using the lexicography order.
If the flag -m is set, perform mathematical evaluation before making comparisons instead.
To end each condition section, use "else"/"endif"
)";

CommandConstraint __constraint_IfCommand()
{
    CommandConstraint constraint(4, 4);
    constraint.add_argument(false, "perform mathematical comparison instead of string comparison", 0, 0, "-m");
    return constraint;
}

class IfCommand : public BaseCommand
{
public:
    IfCommand()
        : BaseCommand(
              "if",
              "Compare strings or math expressions",
              __if_description,
              {},
              __constraint_IfCommand()) {}

    DWORD run(const Context &context)
    {
        auto first = context.args[1], op = context.args[2], second = context.args[3];
        bool force_stream = !context.client->stream.eof();

        unsigned counter = 1;
        std::vector<std::string> if_true;
        bool has_else = false;
        while (true)
        {
            auto input = strip(context.client->stream.getline("if_true>", force_stream ? InputStream::FORCE_STREAM : 0));
            if (startswith(input, "if "))
            {
                counter++;
            }
            else if (input == "else")
            {
                counter--;
                if (counter == 0)
                {
                    has_else = true;
                    break;
                }
            }
            else if (input == "endif")
            {
                counter--;
                if (counter == 0)
                {
                    break;
                }
            }

            if_true.push_back(input);
        }

        counter = 1;
        std::vector<std::string> if_false;
        if (has_else)
        {
            while (true)
            {
                auto input = strip(context.client->stream.getline("if_false>", force_stream ? InputStream::FORCE_STREAM : 0));
                if (startswith(input, "if "))
                {
                    counter++;
                }
                else if (input == "else")
                {
                    counter--;
                    if (counter == 0)
                    {
                        throw std::invalid_argument("\"else\" is not allowed here");
                    }
                }
                else if (input == "endif")
                {
                    counter--;
                    if (counter == 0)
                    {
                        break;
                    }
                }

                if_false.push_back(input);
            }
        }

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

        std::vector<std::string> lines = result ? if_true : if_false;
        context.client->stream.write(lines.begin(), lines.end());

        return 0;
    }
};
