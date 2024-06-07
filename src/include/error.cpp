#include "error.hpp"

namespace liteshell
{
    LiteShellException::LiteShellException(const std::string &message) : message(message) {}

    const char *LiteShellException::what() const noexcept
    {
        return message.c_str();
    }

    SubprocessException::SubprocessException(const std::string &message) : LiteShellException(message) {}
    SubprocessCreationError::SubprocessCreationError(const std::string &message) : SubprocessException(message) {}
    EnvironmentException::EnvironmentException(const std::string &message) : LiteShellException(message) {}
    EnvironmentResolveError::EnvironmentResolveError(const std::string &message) : EnvironmentException(message) {}

    CommandNotFound::CommandNotFound(const std::string &name, const std::string &suggestion)
        : LiteShellException(utils::format("Command \"%s\" not found. Did you mean \"%s\"?", name.c_str(), suggestion.c_str())) {}

    ContextException::ContextException(const std::string &message) : LiteShellException(message) {}

    ArgumentMissingError::ArgumentMissingError(const std::string &name)
        : ContextException(utils::format("Argument \"%s\" is missing", name.c_str())) {}

    UnrecognizedOption::UnrecognizedOption(const std::string &name)
        : ContextException(utils::format("Unrecognized option \"%s\"", name.c_str())) {}

    TooManyPositionalArguments::TooManyPositionalArguments()
        : ContextException("Too many positional arguments were passed") {}
}
