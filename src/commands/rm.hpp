#pragma once

#include <all.hpp>

class RmCommand : public liteshell::BaseCommand
{
public:
    RmCommand()
        : liteshell::BaseCommand(
              "rm",
              "Remove one or many files/directories",
              "If any of the targets is a directory, remove it recursively",
              liteshell::CommandConstraint("targets", "The targets to remove", true, true)) {}

    DWORD run(const liteshell::Context &context)
    {
        for (auto &target : context.values.at("targets"))
        {
            auto targets = utils::list_files(target);
            for (auto &target : targets)
            {
                if (target.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    utils::remove_directory(utils::utf_convert(target.cFileName), true);
                }
                else if (DeleteFileW(target.cFileName))
                {
                    std::cout << "Deleted " << target.cFileName << std::endl;
                }
                else
                {
                    auto message = utils::last_error(utils::format("Error deleting file \"%s\"", utils::utf_convert(target.cFileName).c_str()));
                    std::cerr << message << std::endl;
                }
            }
        }

        return 0;
    }
};
