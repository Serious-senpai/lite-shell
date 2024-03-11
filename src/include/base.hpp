#pragma once

#include "context.hpp"
#include "standard.hpp"

class BaseCommand
{
private:
public:
    const std::string name, description, syntax;
    const std::vector<std::string> aliases;

    BaseCommand(
        const std::string &name,
        const std::string &description,
        const std::string &syntax,
        const std::initializer_list<std::string> &aliases)
        : name(name),
          description(description),
          syntax(syntax),
          aliases(aliases) {}

    virtual ~BaseCommand() {}

    virtual int run(const Context &context) = 0;
};