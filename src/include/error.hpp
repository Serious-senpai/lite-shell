#pragma once

#include "format.hpp"
#include "standard.hpp"

namespace liteshell
{
    /** @brief Base class for specific exceptions from this application */
    class LiteShellException : public std::exception
    {
    public:
        /** @brief The error message */
        const std::string message;

        LiteShellException(const std::string &message);

        const char *what() const noexcept;
    };

    /** @brief Exceptions regarding subprocesses */
    class SubprocessException : public LiteShellException
    {
    public:
        SubprocessException(const std::string &message);
    };

    /** @brief Exception thrown when a subprocess cannot be created */
    class SubprocessCreationError : public SubprocessException
    {
    public:
        SubprocessCreationError(const std::string &message);
    };

    /** @brief Exceptions regarding shell environment */
    class EnvironmentException : public LiteShellException
    {
    public:
        EnvironmentException(const std::string &message);
    };

    /** @brief Exception thrown when resolving environment variables failed */
    class EnvironmentResolveError : public EnvironmentException
    {
    public:
        EnvironmentResolveError(const std::string &message);
    };

    /** @brief Exception thrown when a command couldn't be found */
    class CommandNotFound : public LiteShellException
    {
    public:
        CommandNotFound(const std::string &name, const std::string &suggestion);
    };

    /** @brief Exceptions regarding context parsing */
    class ContextException : public LiteShellException
    {
    public:
        ContextException(const std::string &message);
    };

    /** @brief Exception thrown when a required argument is missing */
    class ArgumentMissingError : public ContextException
    {
    public:
        ArgumentMissingError(const std::string &name);
    };

    /** @brief Exception thrown when an unrecognized option is provided */
    class UnrecognizedOption : public ContextException
    {
    public:
        UnrecognizedOption(const std::string &name);
    };

    /** @brief Exception thrown when the number of provided positional arguments exceeds the constraint */
    class TooManyPositionalArguments : public ContextException
    {
    public:
        TooManyPositionalArguments();
    };
}
