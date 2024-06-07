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
        auto file = CreateFileW(
            utils::utf_convert(context.get("file")).c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (file == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                throw std::invalid_argument("The specified file does not exist");
            }

            throw std::runtime_error(utils::last_error("Error when opening file"));
        }

        auto _finalize = utils::Finalize(
            [&file]()
            {
                CloseHandle(file);
            });

        char buffer[LITE_SHELL_BUFFER_SIZE];
        ZeroMemory(buffer, LITE_SHELL_BUFFER_SIZE);
        DWORD read = LITE_SHELL_BUFFER_SIZE;
        while (read == LITE_SHELL_BUFFER_SIZE)
        {
            if (!ReadFile(file, buffer, LITE_SHELL_BUFFER_SIZE, &read, NULL))
            {
                throw std::runtime_error(utils::last_error("Error when reading file"));
            }

            std::cout << std::string(buffer, buffer + read) << std::flush;
        }

        std::cout << std::endl;

        return 0;
    }
};
