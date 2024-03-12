#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <error.hpp>
#include <standard.hpp>

class TypeCommand : public BaseCommand
{
private:
    const DWORD BUFFER_SIZE = 2048;

public:
    using BaseCommand::run;

    TypeCommand() : BaseCommand(
                        "type",
                        "Read a file",
                        "type <file>",
                        {}) {}

    int run(const Context &context)
    {
        if (context.args.size() < 2)
        {
            throw std::invalid_argument("No file to read");
        }

        auto h_file = CreateFileW(
            utf_convert(context.args[1]).c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (h_file == INVALID_HANDLE_VALUE)
        {
            throw_last_error("Error when opening file");
        }

        char buffer[BUFFER_SIZE] = {};
        DWORD read = BUFFER_SIZE;
        while (read == BUFFER_SIZE)
        {
            if (!ReadFile(h_file, buffer, BUFFER_SIZE, &read, NULL))
            {
                throw_last_error("Error when reading file");
            }

            std::cout << std::string(buffer, buffer + read);
            std::cout.flush();
        }

        CloseHandle(h_file);

        std::cout << std::endl;

        return 0;
    }
};