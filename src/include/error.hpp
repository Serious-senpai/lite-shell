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

    class ContextException : public LiteShellException
    {
    public:
        ContextException(const std::string &message) : LiteShellException(message) {}
    };

    class ArgumentMissingError : public ContextException
    {
    public:
        ArgumentMissingError(const std::string &name) : ContextException(utils::format("Argument \"%s\" is missing", name.c_str())) {}
    };

    class UnrecognizedOption : public ContextException
    {
    public:
        UnrecognizedOption(const std::string &name) : ContextException(utils::format("Unrecognized option \"%s\"", name.c_str())) {}
    };

    class TooManyPositionalArguments : public ContextException
    {
    public:
        TooManyPositionalArguments() : ContextException("Too many positional arguments were passed") {}
    };
}
