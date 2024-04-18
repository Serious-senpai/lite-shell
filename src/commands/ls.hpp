#pragma once

#include <all.hpp>

class LsCommand : public liteshell::BaseCommand
{
public:
    LsCommand()
        : liteshell::BaseCommand(
              "ls",
              "Display the content of a directory",
              "",
              {"dir"},
              liteshell::CommandConstraint("dir", "The directory to explore (default: the working directory)", false)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto directory = utils::get_working_directory();
        try
        {
            directory = context.get("dir");
        }
        catch (liteshell::ArgumentMissingError &e)
        {
            // pass
        }

        std::cout << "Exploring " << directory << std::endl;

        utils::Table displayer("Name", "Type", "Size");

        for (const auto &data : utils::explore_directory(directory))
        {
            long double size = ((long double)data.nFileSizeHigh * ((long double)MAXDWORD + 1.0L)) + (long double)data.nFileSizeLow;
            bool is_directory = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            displayer.add_row(
                utils::utf_convert(std::wstring(data.cFileName)),
                is_directory ? "DIR" : "FILE",
                is_directory ? "-" : utils::memory_size(size));
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
