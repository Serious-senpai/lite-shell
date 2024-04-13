#pragma once

#include <all.hpp>

class CatCommand : public liteshell::BaseCommand
{
public:
    CatCommand()
        : liteshell::BaseCommand(
              "cat",
              "Read a file",
              "Displays the content of a text file.",
              {"type"},
              liteshell::CommandConstraint(2, 2))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        auto h_file = CreateFileW(
            utils::utf_convert(context.args[1]).c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (h_file == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error(utils::last_error("Error when opening file"));
        }

        char buffer[BUFFER_SIZE] = {};
        DWORD read = BUFFER_SIZE;
        while (read == BUFFER_SIZE)
        {
            if (!ReadFile(h_file, buffer, BUFFER_SIZE, &read, NULL))
            {
                throw std::runtime_error(utils::last_error("Error when reading file"));
            }

            std::cout << std::string(buffer, buffer + read);
            std::cout.flush();
        }

        CloseHandle(h_file);

        std::cout << std::endl;

        return 0;
    }
};