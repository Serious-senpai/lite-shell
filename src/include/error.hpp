#pragma once

#include "format.hpp"
#include "standard.hpp"

void throw_last_error(const std::string &message)
{
    throw std::runtime_error(format("%s: %d", message.c_str(), GetLastError()));
}

class CommandInputError : public std::exception
{
private:
    const char *message;

public:
    CommandInputError(const std::string &message) : message(message.c_str()) {}

    const char *what()
    {
        return message;
    }
};