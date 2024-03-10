#pragma once

#include <base.hpp>
#include <converter.hpp>
#include <standard.hpp>

class TypeCommand : public BaseCommand
{
private:
    const DWORD BUFFER_SIZE = 2048;

public:
    using BaseCommand::run;

    TypeCommand() : BaseCommand("type") {}

    int run(const ParseResult &arguments) override
    {
        if (arguments.positional_arguments.size() < 2)
        {
            throw std::invalid_argument("No file to read");
        }

        auto h_file = CreateFileW(
            utf_convert(arguments.positional_arguments[1]).c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (h_file == INVALID_HANDLE_VALUE)
        {
            auto code = GetLastError();
            throw std::runtime_error(format("Error opening file: %d", code));
        }

        char buffer[BUFFER_SIZE] = {};
        DWORD read = BUFFER_SIZE;
        while (read == BUFFER_SIZE)
        {
            auto success = ReadFile(
                h_file,
                buffer,
                BUFFER_SIZE,
                &read,
                NULL);

            if (!success)
            {
                auto code = GetLastError();
                throw std::runtime_error(format("Error reading file: %d", code));
            }

            std::cout << std::string(buffer, buffer + read);
            std::cout.flush();
        }

        std::cout << std::endl;

        return 0;
    }
};