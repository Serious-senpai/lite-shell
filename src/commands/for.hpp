#pragma once

#include <base.hpp>
#include <client.hpp>

class ForCommand : public BaseCommand
{
public:
    ForCommand()
        : BaseCommand(
              "for",
              "Iterate the loop variable over a specified integer range.",
              "To end the loop section, type \"endfor\"",
              {},
              CommandConstraint(3, 4)) {}

    DWORD run(const Context &context)
    {
        auto loop_var = context.args[1];
        auto start = 0ll, end = context.client->get_environment()->eval_ll(context.args[2]);
        if (context.args.size() == 4)
        {
            start = end;
            end = context.client->get_environment()->eval_ll(context.args[3]);
        }

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

        for (auto value = end; value >= start; value--)
        {
            context.client->stream.write(lines.begin(), lines.end());
            context.client->stream.write(format("eval %lld -s %s", value, loop_var.c_str()));
        }

        return 0;
    }
};
