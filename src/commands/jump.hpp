#pragma once

#include <all.hpp>

class JumpCommand : public liteshell::BaseCommand
{
public:
    JumpCommand()
        : liteshell::BaseCommand(
              "jump",
              "Skip the input stream to the specified label",
              "",
              {},
              liteshell::CommandConstraint(2, 2)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto label = context.args[1];
        if (label[0] != ':')
        {
            label = ':' + label;
        }

        context.client->get_stream()->jump(label);

        return 0;
    }
};
