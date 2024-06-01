#pragma once

#include <all.hpp>

class JumpCommand : public liteshell::BaseCommand
{
public:
    JumpCommand()
        : liteshell::BaseCommand(
              "jump",
              "Skip the input stream to the specified label",
              "Examples: \"jump end\", \"jump :end\".\n"
              "When reading from batch scripts, a label :EOF will automatically be added to the end.",
              liteshell::CommandConstraint("label", "The label to jump to", true))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        auto label = context.get("label");
        if (label[0] != ':')
        {
            label = ':' + label;
        }

        context.client->get_stream()->jump(label);

        return 0;
    }
};
