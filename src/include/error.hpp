#pragma once

#include "standard.hpp"

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

class CommandInputError : public LiteShellException
{
public:
    CommandInputError(const std::string &message) : LiteShellException(message) {}
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

class SubprocessPipeError : public SubprocessException
{
public:
    SubprocessPipeError(const std::string &message) : SubprocessException(message) {}
};
