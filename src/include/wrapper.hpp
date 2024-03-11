#pragma once

#include "base.hpp"
#include "context.hpp"
#include "standard.hpp"

template <typename T>
class CommandWrapper
{
private:
public:
    // https://stackoverflow.com/a/66010713
    const std::shared_ptr<T> command;

    CommandWrapper(const std::shared_ptr<T> &command) : command(command)
    {
        static_assert(std::is_base_of<BaseCommand, T>::value, "CommandInvoker can only be used for BaseCommand subclasses");
    }

    int run(const Context &context)
    {
        return command->run(context);
    }
};
