#pragma once

/**
 * Wrapper for a command inheriting from BaseCommand.
 * See https://stackoverflow.com/a/66010713 for more details
 *
 * @tparam T The command class, must be a subclass of BaseCommand
 */
template <typename T>
class CommandWrapper
{
private:
public:
    /* A pointer to the underlying command object */
    const std::shared_ptr<T> command;

    CommandWrapper(const std::shared_ptr<T> &command) : command(command)
    {
        static_assert(std::is_base_of<BaseCommand, T>::value, "CommandWrapper can only be used for BaseCommand subclasses");
    }

    /**
     * Invoke the underlying command and return a new errorlevel for the current shell.
     * This method simply invokes `command->run(context)`.
     *
     * @param context The context in which the command is being invoked under.
     * @return The new errorlevel for the shell.
     */
    DWORD run(const Context &context)
    {
        return command->run(context);
    }
};

template <typename T>
bool operator<(const CommandWrapper<T> &lhs, const CommandWrapper<T> &rhs)
{
    return lhs.command->name < rhs.command->name;
}

template <typename T>
bool operator==(const CommandWrapper<T> &lhs, const CommandWrapper<T> &rhs)
{
    return lhs.command->name == rhs.command->name;
}
