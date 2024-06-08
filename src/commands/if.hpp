#pragma once

#include <all.hpp>

class IfCommand : public liteshell::BaseCommand
{
public:
    IfCommand()
        : liteshell::BaseCommand(
              "if",
              "Compare strings or math expressions",
              "<operator> must be one of the values: \"==\", \"!=\", \"<\", \">\", \"<=\", \">=\".\n\n"
              "The strings are compared using the lexicography order.\n"
              "If the flag -m is set, perform mathematical evaluation before making algebra comparisons.\n"
              "To end each condition section, use \"else\"/\"endif\".",
              liteshell::CommandConstraint(
                  "x", "The first value to compare", true,
                  "operator", "The operator to use for comparison", true,
                  "y", "The second value to compare", true)
                  .add_option("-m", "Perform mathematical comparison instead of string comparison", false))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        auto first = context.get("x"), op = context.get("operator"), second = context.get("y");
        bool force_stream = !context.client->get_stream()->eof();

        unsigned counter = 1;
        std::vector<std::string> if_true;
        bool has_else = false;
        while (true)
        {
            auto input = utils::strip(context.client->get_stream()->getline("if_true>", force_stream ? liteshell::InputStream::FORCE_STREAM : 0));
            if (utils::startswith(input, "if "))
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
            else if (utils::startswith(input, "endif"))
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
                auto input = utils::strip(context.client->get_stream()->getline("if_false>", force_stream ? liteshell::InputStream::FORCE_STREAM : 0));
                if (utils::startswith(input, "if "))
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
                else if (utils::startswith(input, "endif"))
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
        context.client->get_stream()->write(lines.begin(), lines.end());

        return 0;
    }
};
