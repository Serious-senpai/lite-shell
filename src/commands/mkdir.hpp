#pragma once

#include <all.hpp>

class MkdirCommand : public liteshell::BaseCommand
{
public:
    MkdirCommand()
        : liteshell::BaseCommand(
              "mkdir",
              "Make a new directory",
              "",
              {"md"},
              liteshell::CommandConstraint("dir", "The name of the new directory", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        if (!CreateDirectoryW(utils::utf_convert(context.get("dir")).c_str(), NULL))
        {
            throw std::runtime_error(utils::last_error("Unable to create directory"));
        }

        return 0;
    }
};
