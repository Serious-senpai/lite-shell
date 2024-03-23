#pragma once

#include <base.hpp>
#include <client.hpp>

class ForCommand : public BaseCommand
{
public:
    ForCommand()
        : BaseCommand(
              "for",
              "Iterate the loop variable over a specified integer range",
              "",
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
            std::cout << "for>";
            std::cout.flush();

            auto input = strip(context.client->getline());
            if (input == "endfor")
            {
                break;
            }

            lines.push_back(input);
        }

        for (auto value = start; value <= end; value++)
        {
            context.client->get_environment()->set_variable(loop_var, std::to_string(value));
            for (const auto &line : lines)
            {
                context.client->process_command(line);
            }
        }

        return 0;
    }
};
