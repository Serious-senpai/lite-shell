#pragma once

#include <base.hpp>
#include <standard.hpp>
#include <tables.hpp>
#include <units.hpp>
#include <utils.hpp>

class LsCommand : public BaseCommand
{
public:
    LsCommand()
        : BaseCommand(
              "ls",
              "List the content of a directory",
              "ls <optional dir>",
              {"dir"}) {}

    int run(const Context &context)
    {
        auto directory = get_working_directory();
        if (context.args.size() == 2)
        {
            directory = context.args[1];
        }
        else if (context.args.size() > 2)
        {
            throw std::invalid_argument("Expected at most 1 argument only");
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
