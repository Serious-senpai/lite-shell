#pragma once

#include <base.hpp>
#include <client.hpp>

class JumpCommand : public BaseCommand
{
public:
    JumpCommand()
        : BaseCommand(
              "jump",
              "Skip the input stream to the specified label",
              "",
              {},
              CommandConstraint(2, 2)) {}

    DWORD run(const Context &context)
    {
        auto label = context.args[1];
        if (label[0] != ':')
        {
            label = ':' + label;
        }

        context.client->stream.jump(label);

        return 0;
    }
};
