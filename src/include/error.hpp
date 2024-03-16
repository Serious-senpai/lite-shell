#pragma once

#include "standard.hpp"

/* Base class for all exceptions of this command shell */
class LiteShellException : public std::exception
{
private:
    const char *message;

public:
    LiteShellException(const std::string &message) : message(message.c_str()) {}

    const char *what()
    {
        return message;
    }
};

class SubprocessException : public LiteShellException
{
public:
    SubprocessException(const std::string &message) : LiteShellException(message) {}
};

class SubprocessCreationError : public LiteShellException
{
public:
    SubprocessCreationError(const std::string &message) : LiteShellException(message) {}
};
