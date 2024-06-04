#pragma once

#include <all.hpp>

class MvCommand : public liteshell::BaseCommand
{
public:
    MvCommand()
        : liteshell::BaseCommand(
              "mv",
              "Moves files and renames files and directories",
              "",
              liteshell::CommandConstraint(
                  "old", "The existing file/directory name", true,
                  "new", "The new file/directory name", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto old_path = context.get("old");
        auto new_path = context.get("new");
        if (!MoveFileW(utils::utf_convert(old_path).c_str(), utils::utf_convert(new_path).c_str()))
        {
            throw std::runtime_error(utils::last_error("MoveFileW ERROR"));
        }

        return 0;
    }
};
