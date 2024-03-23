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

    Note that if a parameter is absent from the command, its value will not present in the map. Therefore, it is sufficient
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

    /* The arguments constraint of this context object */
    const ArgumentsConstraint constraint;

    Context(
        const std::string &message,
        const std::vector<std::string> &tokens,
        const std::vector<std::string> &args,
        const std::map<std::string, std::vector<std::string>> &kwargs,
        const Client *client,
        const ArgumentsConstraint &constraint)
        : message(message),
          tokens(tokens),
          args(args),
          kwargs(kwargs),
          client(client),
          constraint(constraint) {}

    /*
    Return a shallow copy of the current context

    @return A new context with the same information as the current one
    */
    Context copy() const
    {
        return Context(message, tokens, args, kwargs, client, constraint);
    }

    /*
    Parse this context with new constraint.

    @param constraint The new constraint to parse the context with
    @return A new context with the new constraint applied
    */
    Context parse(const ArgumentsConstraint &constraint) const
    {
        return get_context(client, message, constraint);
    }

    /*
    Replace the first argument of the original command message
    @param token the token to replace with
    @return A new Context with the first argument replaced with the given token
     */
    Context replace_call(const std::string &token) const
    {
        if (tokens.empty())
        {
            throw std::runtime_error("No positional arguments to replace call");
        }

        std::string new_message(token);
        new_message += message.substr(tokens[0].size());

        std::vector<std::string> new_tokens(tokens);
        new_tokens[0] = token;

        std::vector<std::string> new_args(args);
        if (!new_args.empty())
        {
            new_args[0] = token;
        }

        return Context(new_message, new_tokens, new_args, kwargs, client, constraint);
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
            return get_context(client, new_message, constraint);
        }

        return copy();
    }

    std::string get_arguments_string() const
    {
        std::string result;
        bool ok = false;
        for (auto &c : message)
        {
            if (!ok && c == ' ')
            {
                ok = true;
                continue;
            }

            if (ok)
            {
                result += c;
            }
        }

        return result;
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

    @param client A pointer to the Client object
    @param message The message to construct the context from
    @param constraint The constraint to parse the context with
    @return A new context object
    */
    static Context get_context(const Client *client, const std::string &message, const ArgumentsConstraint &constraint);
};

Context Context::get_context(const Client *client, const std::string &message, const ArgumentsConstraint &constraint)
{
    auto tokens = split(message);

    std::vector<std::string> args;
    std::map<std::string, std::vector<std::string>> kwargs;

    if (constraint.require_context_parsing)
    {
        std::optional<std::string> current_parameter;
        auto add_token = [&constraint, &args, &kwargs, &current_parameter](const std::string &token)
        {
            if (current_parameter.has_value())
            {
                // keyword argument
                kwargs[*current_parameter].push_back(token);
                if (constraint.arguments_checking && !constraint.has_argument(*current_parameter))
                {
                    throw std::invalid_argument(format("Unknown argument: %s", current_parameter->c_str()));
                }
            }
            else
            {
                // positional argument
                args.push_back(token);
            }
        };

        auto is_math_expression = [](const std::string &token)
        {
            for (auto &c : token)
            {
                if (c == '+' || c == '-' || c == '*' || c == '/' || c == ' ' || ('0' <= c && c <= '9'))
                {
                    continue;
                }

                return false;
            }

            return true;
        };

        for (auto &token : tokens)
        {
            if (token[0] == '-')
            {
                if (token.size() == 1) // Token "-"
                {
                    throw std::invalid_argument("Input pipe is not supported");
                }
                else if (is_math_expression(token)) // Treat it as a numeric value
                {
                    add_token(token);
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
    }

    if (constraint.arguments_checking)
    {
        if (args.size() < constraint.args_bounds.first)
        {
            throw std::invalid_argument(format("Too few positional arguments: %d < %d", args.size(), constraint.args_bounds.first));
        }

        if (args.size() > constraint.args_bounds.second)
        {
            throw std::invalid_argument(format("Too many positional arguments: %d > %d", args.size(), constraint.args_bounds.second));
        }

        for (auto &[name, values] : kwargs)
        {
            auto bounds = constraint.get_bounds(name);
            if (values.size() < bounds.first)
            {
                throw std::invalid_argument(format("Too few values for argument %s: %d < %d", name.c_str(), values.size(), bounds.first));
            }
            if (values.size() > bounds.second)
            {
                throw std::invalid_argument(format("Too many values for argument %s: %d > %d", name.c_str(), values.size(), bounds.second));
            }
        }
    }

    return Context(message, tokens, args, kwargs, client, constraint);
}
