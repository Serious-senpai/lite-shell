#pragma once

#include <all.hpp>

class ForCommand : public liteshell::BaseCommand
{
private:
    unsigned long long _label_counter = 0;

    std::string _make_new_label()
    {
        return ":for_" + std::to_string(_label_counter++) + utils::random(20);
    }

    std::deque<std::string> _get_lines(const liteshell::Context &raw_context)
    {
        const auto stream_ptr = raw_context.client->get_stream();
        std::deque<std::string> lines;
        unsigned counter = 1;
        bool force_stream = !stream_ptr->exhaust();
        while (true)
        {
            auto input = utils::strip(stream_ptr->getline(
                []()
                { std::cout << "for>" << std::flush; },
                force_stream ? liteshell::InputStream::FORCE_STREAM : 0));
            stream_ptr->consume_last();
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

public:
    ForCommand()
        : liteshell::BaseCommand(
              "for",
              "Iterate the loop variable over a specified integer range",
              "Loop the variable in range [x, y) or [y, x) (always from x to y). To end the loop section, type \"endfor\"",
              liteshell::CommandConstraint(
                  "var", "The name of the loop variable", true,
                  "x", "The start of the loop range", true,
                  "y", "The end of the loop range", true))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        // Do NOT evaluate ANYTHING (even environment variables). Copy the exact components to the compiled commands.
        const auto raw_context = liteshell::Context::get_context(
            context.client,
            context.original_message,
            context.original_message,
            context.constraint);

        const auto stream_ptr = raw_context.client->get_stream();
        const auto loop_var = raw_context.get("var");
        const auto start = raw_context.get("x"), end = raw_context.get("y");

        stream_ptr->consume_last();
        auto lines = _get_lines(raw_context);
        auto loop_label = _make_new_label(), end_label = _make_new_label();

        lines.push_front(loop_label);
        lines.push_front(utils::format("eval -ms \"%s\" \"%s\"", loop_var.c_str(), start.c_str()));

        lines.push_back(utils::format("if -m \"%s\" < \"%s\"", start.c_str(), end.c_str()));
        lines.push_back(utils::format("eval -ms \"%s\" \"$%s + 1\"", loop_var.c_str(), loop_var.c_str()));
        lines.push_back(
            utils::format(
                "_if -m \"$%s\" < \"%s\" \"%s\" \"%s\"",
                loop_var.c_str(),
                end.c_str(),
                loop_label.c_str(),
                end_label.c_str()));
        lines.push_back("else");
        lines.push_back(utils::format("eval -ms \"%s\" \"$%s - 1\"", loop_var.c_str(), loop_var.c_str()));
        lines.push_back(
            utils::format(
                "_if -m \"$%s\" > \"%s\" \"%s\" \"%s\"",
                loop_var.c_str(),
                end.c_str(),
                loop_label.c_str(),
                end_label.c_str()));
        lines.push_back("endif");

        lines.push_back(end_label);

        stream_ptr->write(lines.begin(), lines.end());

        return 0;
    }
};
