#pragma once

#include <all.hpp>

class MemoryCommand : public liteshell::BaseCommand
{
public:
    MemoryCommand()
        : liteshell::BaseCommand(
              "memory",
              "Display global memory status",
              "",
              {},
              liteshell::CommandConstraint()) {}

    DWORD run(const liteshell::Context &context)
    {
        auto status = utils::get_global_memory_status();
        auto display = utils::Table("Attribute", "Value");
        display.add_row("Memory usage", std::to_string(status.dwMemoryLoad) + "%");
        display.add_row("Total physical memory", utils::memory_size(status.ullTotalPhys));
        display.add_row("Available physical memory", utils::memory_size(status.ullAvailPhys));

        std::cout << display.display() << std::endl;

        return 0;
    }
};
