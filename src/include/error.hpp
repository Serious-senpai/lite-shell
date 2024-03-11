#pragma once

#include "standard.hpp"

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