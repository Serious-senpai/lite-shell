#pragma once

#include "context.hpp"
#include "standard.hpp"

/*
Abstract base class for all local commands of the shell.

All command must inherit this class.
*/
class BaseCommand
{
private:
public:
    /* The name of the command */
    const std::string name;

    /* A short description of the command that will be shown when running `>help` */
    const std::string description;

    /* A long description of the command that will be shown when running `>help <name>` */
    const std::string long_description;

    /* The syntax of the command that will be shown when running `>help <name>` */
    const std::string syntax;

    /* The list of aliases the command can be invoked under. */
    const std::vector<std::string> aliases;

    BaseCommand(
        const std::string &name,
        const std::string &description,
        const std::string &long_description,
        const std::string &syntax,
        const std::initializer_list<std::string> &aliases)
        : name(name),
          description(description),
          long_description(long_description),
          syntax(syntax),
          aliases(aliases) {}

    virtual ~BaseCommand() {}

    /*
    Invoke this command and return a new errorlevel for the current shell. Subclasses must implement this method.

    @param context The context in which the command is being invoked under.
    @return The new errorlevel for the shell.
    */
    virtual DWORD run(const Context &context) = 0;
};