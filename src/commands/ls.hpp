#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <error.hpp>
#include <standard.hpp>
#include <tables.hpp>
#include <units.hpp>
#include <utils.hpp>

class LsCommand : public BaseCommand
{
public:
    using BaseCommand::run;

    LsCommand()
        : BaseCommand(
              "ls",
              "List the content of a directory",
              "ls <optional dir>",
              {}) {}

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

        directory += "\\*";
        WIN32_FIND_DATAW data;
        HANDLE h_file = FindFirstFileW(utf_convert(directory).c_str(), &data);
        if (h_file == INVALID_HANDLE_VALUE)
        {
            throw_last_error("Error when listing directory");
        }

        do
        {
            long double size = ((long double)data.nFileSizeHigh * ((long double)MAXDWORD + 1.0L)) + (long double)data.nFileSizeLow;
            bool is_directory = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            displayer.add_row(
                {utf_convert(std::wstring(data.cFileName)),
                 is_directory ? "DIR" : "FILE",
                 is_directory ? "-" : memory_size(size)});
        } while (FindNextFileW(h_file, &data));

        std::cout << displayer.display() << std::endl;

        if (!FindClose(h_file))
        {
            throw_last_error("Error when closing file search handle");
        }

        CloseHandle(h_file);

        return 0;
    }
};
