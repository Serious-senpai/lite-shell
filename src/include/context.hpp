#pragma once

#include "error.hpp"
#include "split.hpp"

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

    /* The list of tokens after parsing the message: e.g. `args a b -c d` will give `[args, a, b, -c, d]`. */
    const std::vector<std::string> tokens;

    /* The positional arguments passed to the command: e.g. `args a b -c d` will give `[args, a, b]`. */
    const std::vector<std::string> args;

    /*
    The values of named arguments passed to the command.
    Suppose there is a command `args` with a 2 alias groups (`-a`, `-b`) and (`-c`, `-d`), calling `args -a 1 -b 2 -c 3 4 5`
    will give a mapping of `{-a: [1, 2], -b: [1, 2], -c: [3, 4, 5], -d: [3, 4, 5]}`
    */
    const std::map<std::string, std::vector<std::string>> kwargs;

    /*
    The named arguments presenting in the command.
    This can be used to detect which alias of a named argument was called: e.g. if `-a` and `-b` are 2 aliases, calling
    `args 1 2 -a` will only give `{-a}`
    */
    const std::set<std::string> present;

    /* A pointer to the client that contains the command being executed. */
    class Client *const client;

    /* The arguments constraint of this context object */
    const CommandConstraint constraint;

    Context(
        const std::string &message,
        const std::vector<std::string> &tokens,
        const std::vector<std::string> &args,
        const std::map<std::string, std::vector<std::string>> &kwargs,
        const std::set<std::string> &present,
        Client *const client,
        const CommandConstraint &constraint)
        : message(message),
          tokens(tokens),
          args(args),
          kwargs(kwargs),
          present(present),
          client(client),
          constraint(constraint) {}

    /*
    Return a shallow copy of the current context

    @return A new context with the same information as the current one
    */
    Context copy() const
    {
        return Context(message, tokens, args, kwargs, present, client, constraint);
    }

    /*
    Parse this context with new constraint.

    @param constraint The new constraint to parse the context with
    @return A new context with the new constraint applied
    */
    Context parse(const CommandConstraint &constraint) const
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

        return Context(new_message, new_tokens, new_args, kwargs, present, client, constraint);
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

    /*
    Get the command string, excluding the first token.

    @return The command without the first token
    */
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
    static Context get_context(Client *const client, const std::string &message, const CommandConstraint &constraint);
};

Context Context::get_context(Client *const client, const std::string &message, const CommandConstraint &constraint)
{
    auto tokens = split(message);

    std::vector<std::string> args;

    std::vector<std::vector<std::string>> kwargs;
    std::vector<bool> present;
    std::map<std::string, unsigned> kwargs_map;

    std::set<std::string> construct_present;
    std::map<std::string, std::vector<std::string>> construct_kwargs;

    if (constraint.arguments_checking)
    {
        for (auto &aliases : constraint.get_alias_groups())
        {
            kwargs.emplace_back();
            present.push_back(false);
            for (auto &alias : aliases)
            {
                kwargs_map[alias] = kwargs.size() - 1;
            }
        }
    }

    if (constraint.require_context_parsing)
    {
        std::optional<std::string> current_parameter;
        auto validate_name = [&constraint, &present, &kwargs_map, &construct_present, &current_parameter](const std::string &name)
        {
            if (constraint.arguments_checking && !constraint.has_argument(name))
            {
                throw std::invalid_argument(format("Unknown argument: %s", name.c_str()));
            }

            current_parameter = name;

            if (constraint.arguments_checking)
            {
                present[kwargs_map[name]] = true;
            }
            construct_present.insert(name);
        };

        auto add_token = [&constraint, &args, &kwargs, &kwargs_map, &construct_kwargs, &current_parameter](const std::string &token)
        {
            if (current_parameter.has_value())
            {
                // keyword argument
                if (constraint.arguments_checking)
                {
                    kwargs[kwargs_map[*current_parameter]].push_back(token);
                }
                else
                {
                    construct_kwargs[*current_parameter].push_back(token);
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
                if (!is_math_symbol(c))
                {
                    return false;
                }
            }

            return true;
        };

        for (auto &token : tokens)
        {
            // std::cout << "Parsing " << token << std::endl;
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

                        validate_name(name);
                    }
                }
                else // Token of type "--abc"
                {
                    validate_name(token);
                }
            }
            else
            {
                add_token(token);
            }
        }

        if (constraint.arguments_checking)
        {
            auto bounds = constraint.args_bounds;
            if (args.size() == 0)
            {
                throw std::runtime_error("This should never happen: args.size() == 0");
            }

            if (args.size() < bounds.first)
            {
                throw std::invalid_argument(format("Too few positional arguments: %d < %d", args.size() - 1, bounds.first - 1));
            }

            if (args.size() > bounds.second)
            {
                throw std::invalid_argument(format("Too many positional arguments: %d > %d", args.size() - 1, bounds.second - 1));
            }

            for (auto &aliases : constraint.get_alias_groups())
            {
                std::vector<std::string> values;
                for (auto &alias : aliases)
                {
                    for (auto &value : kwargs[kwargs_map[alias]])
                    {
                        values.push_back(value);
                    }
                }

                for (auto &alias : aliases)
                {
                    construct_kwargs[alias] = values;
                }
            }

            for (auto &aliases : constraint.get_alias_groups())
            {
                if (aliases.empty())
                {
                    throw std::runtime_error("Unexpected empty alias group");
                }

                auto name = *aliases.begin();
                auto c = constraint.get_constraint(name);
                auto aliases_display = join(aliases.begin(), aliases.end(), "|").c_str();

                if (c.required && !present[kwargs_map[name]])
                {
                    throw std::invalid_argument(format("Missing required argument: %s", aliases_display));
                }

                if (present[kwargs_map[name]])
                {
                    auto size = kwargs[kwargs_map[name]].size();
                    if (size < c.bounds.first)
                    {
                        throw std::invalid_argument(format("Too few values for argument %s: %d < %d", aliases_display, size, c.bounds.first));
                    }
                    if (size > c.bounds.second)
                    {
                        throw std::invalid_argument(format("Too many values for argument %s: %d > %d", aliases_display, size, c.bounds.second));
                    }
                }
            }
        }
    }

    return Context(message, tokens, args, construct_kwargs, construct_present, client, constraint);
}
