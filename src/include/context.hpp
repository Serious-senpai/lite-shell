#pragma once

#include "format.hpp"
#include "standard.hpp"

// Forward declaration
class Client;

class Context
{
private:
public:
    // The original message
    const std::string message;

    // The positional arguments
    const std::vector<std::string> args;

    // The keyword arguments
    const std::map<std::string, std::vector<std::string>> kwargs;

    // The underlying client
    const Client *client;

    Context(
        const std::string &message,
        const std::vector<std::string> &args,
        const std::map<std::string, std::vector<std::string>> &kwargs,
        const Client *client)
        : message(message),
          args(args),
          kwargs(kwargs),
          client(client) {}

    Context replace_call(const std::string &token)
    {
        if (args.empty())
        {
            throw std::runtime_error("No positional arguments to replace call");
        }

        std::string new_message(token);
        new_message += message.substr(args[0].size());

        std::vector<std::string> new_args(args);
        new_args[0] = token;

        return Context(new_message, new_args, kwargs, client);
    }
};
