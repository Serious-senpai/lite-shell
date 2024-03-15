#pragma once

#include "format.hpp"
#include "standard.hpp"

class Client; // Forward declaration

/*
Represents the context in which a command is being invoked under.

This class contains a lot of meta data to help you understand more about the invocation context. This
class is not created manually and is instead passed around to commands as the only parameter.
 */
class Context
{
private:
public:
    /* The message that triggered the command being executed. */
    const std::string message;

    /* The list of tokens after parsing the message: e.g. `args a b -c d` will give [`args`, `a`, `b`, `-c`, `d`]. */
    const std::vector<std::string> tokens;

    /* The positional arguments passed to the command: e.g. `args a b -c d` will give [`a`, `b`]. */
    const std::vector<std::string> args;

    /*
    The keyword arguments passed to the command: e.g. `args a b -c d e -g h` will give {`-c`: [`-c`, `d`, `e`],
    `-g`: [`-g`, `h`]}.

    Note that if a parameter is absent from the command, its value will be an empty vector. Therefore, it is sufficient
    to check the existence of a paramater (e.g. `-v`) by the following idiom:

```cpp
        if (context.kwargs.count("-v")) {
            // -v is present
        } else{
            // -v is absent
        }
```
    */
    const std::map<std::string, std::vector<std::string>> kwargs;

    /* A pointer to the client that contains the command being executed. */
    const Client *client;

    Context(
        const std::string &message,
        const std::vector<std::string> &tokens,
        const std::vector<std::string> &args,
        const std::map<std::string, std::vector<std::string>> &kwargs,
        const Client *client)
        : message(message),
          tokens(tokens),
          args(args),
          kwargs(kwargs),
          client(client) {}

    /*
    @brief Replace the first argument of the original command message
    @param token the token to replace with
    @return A new Context with the first argument replaced with the given token
     */
    Context replace_call(const std::string &token)
    {
        if (tokens.empty() || args.empty())
        {
            throw std::runtime_error("No positional arguments to replace call");
        }

        std::string new_message(token);
        new_message += message.substr(args[0].size());

        std::vector<std::string> new_tokens(tokens);
        new_tokens[0] = token;

        std::vector<std::string> new_args(args);
        new_args[0] = token;

        return Context(new_message, new_tokens, new_args, kwargs, client);
    }
};
