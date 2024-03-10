#pragma once

#include "base.hpp"
#include "parse_results.hpp"
#include "standard.hpp"

template <typename T>
class CommandInvoker
{
private:
    // https://stackoverflow.com/a/66010713
    std::shared_ptr<T> object;

public:
    const std::string name;

    CommandInvoker(const std::shared_ptr<T> object) : object(object), name(object->name)
    {
        static_assert(std::is_base_of<BaseCommand, T>::value, "CommandInvoker can only be used for BaseCommand subclasses");
    }

    ParseResult parse(const std::vector<std::string> &args)
    {
        return object->parse(args);
    }

    int run(const ParseResult &arguments)
    {
        return object->run(arguments);
    }
};
