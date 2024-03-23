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
              ArgumentsConstraint(3, 4)) {}

    DWORD run(const Context &context)
    {
        auto loop_var = context.args[1];
        auto start = 0ll, end = std::stoll(context.args[2]);
        if (context.args.size() == 4)
        {
            start = end;
            end = std::stoll(context.args[3]);
        }

        std::vector<std::string> lines;
        while (true)
        {
            if (context.client->stream.echo)
            {
                std::cout << "for>";
                std::cout.flush();
            }

            auto input = strip(context.client->stream.getline());
            if (input == "endfor")
            {
                break;
            }

            lines.push_back(input);
        }

        std::reverse(lines.begin(), lines.end());

        for (auto value = end; value >= start; value--)
        {
            for (const auto &line : lines)
            {
                context.client->stream.write_front(line);
            }

            context.client->stream.write_front(format("eval %lld -s %s", value, loop_var.c_str()));
        }

        return 0;
    }
};
