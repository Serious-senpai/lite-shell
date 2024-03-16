#pragma once

#include "error.hpp"
#include "format.hpp"
#include "standard.hpp"
#include "utils.hpp"

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
    /* A suffix indicating that a command message should be run in a background */
    const char BACKGROUND_SUFFIX = '%';

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
    Return a shallow copy of the current context

    @return A new context with the same information as the current one
    */
    Context copy() const
    {
        return Context(message, tokens, args, kwargs, client);
    }

    /*
    Replace the first argument of the original command message
    @param token the token to replace with
    @return A new Context with the first argument replaced with the given token
     */
    Context replace_call(const std::string &token) const
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

    /*
    Remove the background suffix token from the command message

    @return A new context with the background suffix token removed
    */
    Context strip_background_request() const
    {
        if (is_background_request())
        {
            auto size = message.size();
            while (message[size - 1] != BACKGROUND_SUFFIX)
            {
                size--;
            }

            auto new_message = message.substr(0, size - 1);
            return get_context(client, new_message);
        }

        return copy();
    }

    /*
    Determine whether this context is requesting to run in a background process.

    @return `true` if the context is requesting to run in the background, `false` otherwise
    */
    bool is_background_request() const
    {
        return !tokens.empty() && tokens.back() == std::string(1, BACKGROUND_SUFFIX); // copy constructor
    }

    /*
    Construct a Context from a message

    @param message The message to construct the context from
    @return A new context object
    */
    static Context get_context(const Client *client, const std::string &message);
};

Context Context::get_context(const Client *client, const std::string &message)
{
    auto tokens = split(message);

    std::vector<std::string> args;
    std::map<std::string, std::vector<std::string>> kwargs;

    std::optional<std::string> current_parameter;
    auto add_token = [&args, &kwargs, &current_parameter](const std::string &token)
    {
        if (current_parameter.has_value())
        {
            // keyword argument
            kwargs[*current_parameter].push_back(token);
        }
        else
        {
            // positional argument
            args.push_back(token);
        }
    };

    for (auto &token : tokens)
    {
        if (token[0] == '-')
        {
            if (token.size() == 1) // Token "-"
            {
                throw std::invalid_argument("Input pipe is not supported");
            }
            else if (token[1] != '-') // Token of type "-abc", treat it as "-a", "-b", "-c"
            {
                for (unsigned i = 1; i < token.size(); i++)
                {
                    std::string name = "-";
                    name += token[i];

                    if (token[i] < 'a' || token[i] > 'z')
                    {
                        throw std::invalid_argument(format("Unsupported option: %s", name.c_str()));
                    }

                    current_parameter = name;
                    add_token(name);
                }
            }
            else // Token of type "--abc"
            {
                current_parameter = token;
                add_token(token);
            }
        }
        else
        {
            add_token(token);
        }
    }

    return Context(message, tokens, args, kwargs, client);
}
