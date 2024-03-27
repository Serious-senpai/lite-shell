#pragma once

#include <all.hpp>

const std::set<std::string> __for_types = {"range", "split"};

CommandConstraint __constraint_ForCommand()
{
    CommandConstraint constraint(3, 4);
    constraint.add_argument(
        "-t", "--type",
        true,
        "The type of loop, must be one of " + join(__for_types.begin(), __for_types.end(), "/"),
        1, 1);
    return constraint;
}

class ForCommand : public BaseCommand
{
public:
    ForCommand()
        : BaseCommand(
              "for",
              "Iterate the loop variable over a specified integer range.",
              "To end the loop section, type \"endfor\"",
              {},
              __constraint_ForCommand()) {}

    std::vector<std::string> get_lines(const Context &context)
    {
        std::vector<std::string> lines;
        unsigned counter = 1;
        bool force_stream = !context.client->stream.eof();
        while (true)
        {
            auto input = strip(context.client->stream.getline("for>", force_stream ? InputStream::FORCE_STREAM : 0));
            if (startswith(input, "for "))
            {
                counter++;
            }
            else if (input == "endfor")
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
        const Context &context,
        const std::string &loop_var,
        std::vector<std::string> &loop_values,
        const std::vector<std::string> &lines)
    {
        if (!loop_values.empty())
        {
            if (lines.empty()) // No-op loops, reduce to a single assignment
            {
                context.client->stream.write(format("eval %s -s %s", loop_values.back().c_str(), loop_var.c_str()));
            }
            else
            {
                std::reverse(loop_values.begin(), loop_values.end());
                for (const auto &value : loop_values)
                {
                    context.client->stream.write(lines.begin(), lines.end());
                    context.client->stream.write(format("eval %s -s %s", value.c_str(), loop_var.c_str()));
                }
            }
        }
    }

    DWORD run(const Context &context)
    {
        auto type = context.kwargs.at("-t")[0];
        if (type == "range")
        {
            auto loop_var = context.args[1];
            auto start = 0ll, end = context.client->get_environment()->eval_ll(context.args[2]);
            if (context.args.size() == 4)
            {
                start = end;
                end = context.client->get_environment()->eval_ll(context.args[3]);
            }

            std::vector<std::string> loop_values;
            for (long long value = start; value < end; value++)
            {
                loop_values.push_back(std::to_string(value));
            }

            insert_commands(context, loop_var, loop_values, get_lines(context));
        }
        else if (type == "split")
        {
            auto loop_var = context.args[1];
            auto values = split(context.args[2], ' ');
            insert_commands(context, loop_var, values, get_lines(context));
        }

        return 0;
    }
};
