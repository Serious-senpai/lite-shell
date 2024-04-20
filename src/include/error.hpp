#pragma once

namespace liteshell
{
    /** @brief Base class for specific exceptions from this application */
    class LiteShellException : public std::exception
    {
    public:
        /** @brief The error message */
        const std::string message;

        LiteShellException(const std::string &message) : message(message) {}

        const char *what() const noexcept
        {
            return message.c_str();
        }
    };

    /** @brief Exceptions regarding subprocesses */
    class SubprocessException : public LiteShellException
    {
    public:
        SubprocessException(const std::string &message) : LiteShellException(message) {}
    };

    /** @brief Exception thrown when a subprocess cannot be created */
    class SubprocessCreationError : public SubprocessException
    {
    public:
        SubprocessCreationError(const std::string &message) : SubprocessException(message) {}
    };

    /** @brief Exceptions regarding shell environment */
    class EnvironmentException : public LiteShellException
    {
    public:
        EnvironmentException(const std::string &message) : LiteShellException(message) {}
    };

    /** @brief Exception thrown when resolving environment variables failed */
    class EnvironmentResolveError : public EnvironmentException
    {
    public:
        EnvironmentResolveError(const std::string &message) : EnvironmentException(message) {}
    };

    /** @brief Exception thrown when a command couldn't be found */
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
