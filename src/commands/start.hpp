#pragma once

#include <all.hpp>

class StartCommand : public liteshell::BaseCommand
{
public:
    StartCommand()
        : liteshell::BaseCommand(
              "start",
              "Starts a new window of the command shell, run the specified command and exit.",
              "",
              liteshell::CommandConstraint("command", "The command line to run", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto command = context.get("command");
        context.client->spawn_subprocess(
            utils::format("%s \"%s\"", utils::get_executable_path().c_str(), command.c_str()),
            true, // no effect
            true);

        return 0;
    }
};
