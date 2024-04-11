#pragma once

namespace liteshell
{
    class LiteShellException : public std::exception
    {
    public:
        const std::string message;

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
        CommandNotFound(const std::string &name, const std::string &suggestion)
            : LiteShellException(utils::format("Command \"%s\" not found. Did you mean \"%s\"?", name.c_str(), suggestion.c_str())) {}
    };
}
