#pragma once

#include "base.hpp"
#include "context.hpp"
#include "standard.hpp"

/*
Wrapper for a command inheriting from BaseCommand.
See https://stackoverflow.com/a/66010713 for more details

@tparam T The command class, must be a subclass of BaseCommand
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

    DWORD run(const Context &context)
    {
        return command->run(context);
    }
};
