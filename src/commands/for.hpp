#pragma once

#include <all.hpp>

class ForCommand : public liteshell::BaseCommand
{
public:
    ForCommand()
        : liteshell::BaseCommand(
              "for",
              "Iterate the loop variable over a specified integer range or string tokens.",
              "To end the loop section, type \"endfor\"",
              liteshell::CommandConstraint(
                  "var", "The name of the loop variable", true,
                  "x", "The start of the loop range, or the string to split", true,
                  "y", "The end of the loop range if iterating over an integer range", false)
                  .add_option(
                      "-t", "--type",
                      "The type of loop",
                      liteshell::PositionalArgument(
                          "type",
                          "Must be \"range\" or \"split\" \n"
                          "If \"range\" is specified, loop the variable in range [x, y) or [y, x) (from x to y)\n"
                          "If \"split\" is specified, split the string by spaces and loop the variable over the tokens",
                          false, true),
                      true))
    {
    }

    std::vector<std::string> get_lines(const liteshell::Context &context)
    {
        std::vector<std::string> lines;
        unsigned counter = 1;
        bool force_stream = !context.client->get_stream()->exhaust();
        while (true)
        {
            auto input = utils::strip(context.client->get_stream()->getline(
                []()
                { std::cout << "for>" << std::flush; },
                force_stream ? liteshell::InputStream::FORCE_STREAM : 0));
            if (utils::startswith(input, "for "))
            {
                counter++;
            }
            else if (utils::startswith(input, "endfor"))
            {
                counter--;
                if (counter == 0)
                {
                    break;
                }
            }

            lines.push_back(input);
        }

        return lines;
    }

    void insert_commands(
        const liteshell::Context &context,
        const std::string &loop_var,
        const std::vector<std::string> &loop_values,
        const std::vector<std::string> &lines)
    {
        if (!loop_values.empty())
        {
            if (lines.empty()) // No-op loops, reduce to a single assignment
            {
                context.client->get_stream()->write(utils::format("eval \"%s\" -s %s", loop_values.back().c_str(), loop_var.c_str()), true);
            }
            else
            {
                std::vector<std::string> _loop_values(loop_values);
                std::reverse(_loop_values.begin(), _loop_values.end());
                for (const auto &value : _loop_values)
                {
                    context.client->get_stream()->write(lines.begin(), lines.end(), true);
                    context.client->get_stream()->write(utils::format("eval \"%s\" -s %s", value.c_str(), loop_var.c_str()), true);
                }
            }
        }
    }

    DWORD run(const liteshell::Context &context)
    {
        auto type = context.get("-t type");
        auto loop_var = context.get("var");
        if (type == "range")
        {
            auto start = context.client->get_environment()->eval_ll(context.get("x")),
                 end = context.client->get_environment()->eval_ll(context.get("y"));

            std::vector<std::string> loop_values;
            if (start < end)
            {
                for (long long value = start; value < end; value++)
                {
                    loop_values.push_back(std::to_string(value));
                }
            }
            else if (start > end)
            {
                for (long long value = start; value > end; value--)
                {
                    loop_values.push_back(std::to_string(value));
                }
            }

            insert_commands(context, loop_var, loop_values, get_lines(context));
        }
        else if (type == "split")
        {
            auto values = utils::split(context.get("x"), ' ');
            insert_commands(context, loop_var, values, get_lines(context));
        }

        return 0;
    }
};
