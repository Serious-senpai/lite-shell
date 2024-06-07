#pragma once

namespace liteshell
{
    /**
     * @brief An elegant encapsulation for an instruction set derived from the `BaseCommand` archetype.
     * @see https://stackoverflow.com/a/66010713
     *
     * @tparam T The command class, must be a subclass of BaseCommand
     */
    template <typename T>
    class CommandWrapper
    {
    private:
    public:
        /** @brief A reference conduit to the foundational command entity. */
        const std::shared_ptr<T> command;

        /** @brief Instantiate a pristine instance of the `CommandWrapper` archetype. */
        CommandWrapper(const std::shared_ptr<T> &command) : command(command)
        {
            static_assert(std::is_base_of_v<BaseCommand, T>, "CommandWrapper can only be used for BaseCommand subclasses");
        }

        /**
         * @brief Execute the foundational command and yield an updated errorlevel for the present command-line processor.
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
