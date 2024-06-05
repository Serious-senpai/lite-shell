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
              liteshell::CommandConstraint("file", "The file to read", true))
    {
    }

    DWORD run(const liteshell::Context &context)
    {
        auto h_file = CreateFileW(
            utils::utf_convert(context.get("file")).c_str(),
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

        char buffer[LITE_SHELL_BUFFER_SIZE];
        ZeroMemory(buffer, LITE_SHELL_BUFFER_SIZE);
        DWORD read = LITE_SHELL_BUFFER_SIZE;
        while (read == LITE_SHELL_BUFFER_SIZE)
        {
            if (!ReadFile(h_file, buffer, LITE_SHELL_BUFFER_SIZE, &read, NULL))
            {
                throw std::runtime_error(utils::last_error("Error when reading file"));
            }

            std::cout << std::string(buffer, buffer + read) << std::flush;
        }

        CloseHandle(h_file);

        std::cout << std::endl;

        return 0;
    }
};