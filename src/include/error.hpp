#pragma once

class LiteShellException : public std::exception
{
private:
    const std::string message;

public:
    LiteShellException(const std::string &message) : message(message) {}

    const char *what() const noexcept
    {
        return message.c_str();
    }
};

class SubprocessException : public LiteShellException
{
public:
    SubprocessException(const std::string &message) : LiteShellException(message) {}
};

class SubprocessCreationError : public SubprocessException
{
public:
    SubprocessCreationError(const std::string &message) : SubprocessException(message) {}
};

class EnvironmentException : public LiteShellException
{
public:
    EnvironmentException(const std::string &message) : LiteShellException(message) {}
};

class EnvironmentResolveError : public EnvironmentException
{
public:
    EnvironmentResolveError(const std::string &message) : EnvironmentException(message) {}
};

class CommandNotFound : public LiteShellException
{
public:
    const std::string name;

    CommandNotFound(const std::string &name)
        : LiteShellException(format("Command \"%s\" not found", name.c_str())),
          name(name) {}
};
