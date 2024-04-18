#pragma once

#include <all.hpp>

const char *__week_days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

class DateCommand : public liteshell::BaseCommand
{
public:
    DateCommand()
        : liteshell::BaseCommand(
              "date",
              "Retrieves the current system date and time",
              "",
              {},
              liteshell::CommandConstraint()) {}

    DWORD run(const liteshell::Context &context)
    {
        SYSTEMTIME time;

        GetSystemTime(&time);
        std::cout << "System time (UTC): "
                  << __week_days[time.wDayOfWeek] << " " << time.wDay << "/" << time.wMonth << "/" << time.wYear << " "
                  << time.wHour << ":" << time.wMinute << ":" << time.wSecond << std::endl;

        GetLocalTime(&time);
        std::cout << "Local time: "
                  << __week_days[time.wDayOfWeek] << " " << time.wDay << "/" << time.wMonth << "/" << time.wYear << " "
                  << time.wHour << ":" << time.wMinute << ":" << time.wSecond << std::endl;

        return 0;
    }
};
