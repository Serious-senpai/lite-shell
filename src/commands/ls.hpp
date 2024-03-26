#pragma once

#include <all.hpp>

class LsCommand : public BaseCommand
{
public:
    LsCommand()
        : BaseCommand(
              "ls",
              "Display the content of a directory (default: the working directory)",
              "",
              {"dir"},
              CommandConstraint(1, 2)) {}

    DWORD run(const Context &context)
    {
        auto directory = get_working_directory();
        if (context.args.size() == 2)
        {
            directory = context.args[1];
        }

        std::cout << "Exploring " << directory << std::endl;

        Table displayer({"Name", "Type", "Size"});

        for (const auto &data : explore_directory(directory))
        {
            long double size = ((long double)data.nFileSizeHigh * ((long double)MAXDWORD + 1.0L)) + (long double)data.nFileSizeLow;
            bool is_directory = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            displayer.add_row(
                {utf_convert(std::wstring(data.cFileName)),
                 is_directory ? "DIR" : "FILE",
                 is_directory ? "-" : memory_size(size)});
        }

        std::cout << displayer.display() << std::endl;

        return 0;
    }
};
