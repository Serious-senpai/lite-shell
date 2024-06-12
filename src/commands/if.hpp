#pragma once

#include <all.hpp>

class IfCommand : public liteshell::BaseCommand
{
private:
    unsigned long long _label_counter = 0;

    std::string _make_new_label()
    {
        return ":if_" + std::to_string(_label_counter++) + utils::random(20);
    }

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
        // Do NOT evaluate ANYTHING (even environment variables). Copy the exact components to the compiled commands.
        const auto raw_context = liteshell::Context::get_context(
            context.client,
            context.original_message,
            context.original_message,
            context.constraint);
        const auto stream_ptr = raw_context.client->get_stream();
        bool force_stream = !stream_ptr->exhaust();

        unsigned counter = 1;
        std::deque<std::string> if_true;
        bool has_else = false;

        stream_ptr->consume_last();
        while (true)
        {
            auto input = utils::strip(stream_ptr->getline(
                []()
                { std::cout << "if_true>" << std::flush; },
                force_stream ? liteshell::InputStream::FORCE_STREAM : 0));
            stream_ptr->consume_last();
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
        std::deque<std::string> if_false;
        if (has_else)
        {
            while (true)
            {
                auto input = utils::strip(stream_ptr->getline(
                    []()
                    { std::cout << "if_false>" << std::flush; },
                    force_stream ? liteshell::InputStream::FORCE_STREAM : 0));
                stream_ptr->consume_last();
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

        const auto restore_echo = stream_ptr->echo() ? liteshell::InputStream::ECHO_ON : liteshell::InputStream::ECHO_OFF;
        const auto true_label = _make_new_label(), false_label = _make_new_label(), end_label = _make_new_label();

        if_true.push_front(restore_echo);
        if_true.push_front(true_label);
        if_true.push_back(liteshell::InputStream::ECHO_OFF);
        if_true.push_back("jump " + end_label);

        if_false.push_front(restore_echo);
        if_false.push_front(false_label);
        if_false.push_back(liteshell::InputStream::ECHO_OFF);
        if_false.push_back("jump " + end_label);

        stream_ptr->write(restore_echo);
        stream_ptr->write(end_label);
        stream_ptr->write(if_false.begin(), if_false.end());
        stream_ptr->write(if_true.begin(), if_true.end());

        auto x = raw_context.get("x"), op = raw_context.get("operator"), y = raw_context.get("y");
        stream_ptr->write(
            utils::format(
                "_if %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
                raw_context.present.count("-m") ? "-m" : "",
                x.c_str(), op.c_str(), y.c_str(),
                true_label.c_str(), false_label.c_str()));
        stream_ptr->write(liteshell::InputStream::ECHO_OFF);

        return 0;
    }
};
