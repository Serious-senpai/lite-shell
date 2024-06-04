#pragma once

namespace liteshell
{
    /**
     * @brief Wrapper for a command inheriting from `BaseCommand`.
     * @see https://stackoverflow.com/a/66010713
     *
     * @tparam T The command class, must be a subclass of BaseCommand
     */
    template <typename T>
    class CommandWrapper
    {
    private:
    public:
        /** @brief A pointer to the underlying command object */
        const std::shared_ptr<T> command;

        /** @brief Construct a new `CommandWrapper` object */
        CommandWrapper(const std::shared_ptr<T> &command) : command(command)
        {
            static_assert(std::is_base_of_v<BaseCommand, T>, "CommandWrapper can only be used for BaseCommand subclasses");
        }

        /**
         * @brief Invoke the underlying command and return a new errorlevel for the current shell.
         *
         * This method simply invokes `command->run(context)`.
         *
         * @see `BaseCommand::run`
         *
         * @param context The context in which the command is being invoked under.
         * @return The new errorlevel for the shell.
         */
        DWORD run(const Context &context)
        {
            return command->run(context);
        }
    };

    /** @brief Comparator to use within std::set and std::map */
    template <typename T>
    bool operator<(const CommandWrapper<T> &lhs, const CommandWrapper<T> &rhs)
    {
        return lhs.command->name < rhs.command->name;
    }

    /** @brief Comparator to use within std::set and std::map */
    template <typename T>
    bool operator==(const CommandWrapper<T> &lhs, const CommandWrapper<T> &rhs)
    {
        return lhs.command->name == rhs.command->name;
    }
}