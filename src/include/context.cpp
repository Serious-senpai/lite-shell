#include "context.hpp"

namespace liteshell
{
    Context::Context(
        const std::string &message,
        const std::vector<std::string> &tokens,
        const std::map<std::string, std::vector<std::string>> &values,
        const std::set<std::string> &present,
        const std::shared_ptr<class Client> &client,
        const std::optional<CommandConstraint> &constraint)
        : message(message),
          tokens(tokens),
          values(values),
          present(present),
          client(client),
          constraint(constraint) {}

    std::string Context::get(const std::string &name) const
    {
#ifdef DEBUG
        std::cout << "Getting argument \"" << name << "\"" << std::endl;
#endif

        auto iter = values.find(name);
        if (iter == values.end())
        {
            throw ArgumentMissingError(name);
        }

        return iter->second[0];
    }

    Context Context::parse(const std::optional<CommandConstraint> &constraint) const
    {
        return get_context(client, message, constraint);
    }

    Context Context::replace_call(const std::string &token) const
    {
        if (tokens.empty())
        {
            throw std::runtime_error("No positional arguments to replace call");
        }

        std::string new_message(token);
        new_message += message.substr(tokens[0].size());

        std::vector<std::string> new_tokens(tokens);
        new_tokens[0] = token;

        return Context(new_message, new_tokens, values, present, client, constraint);
    }

    Context Context::strip_background_request() const
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

        return *this;
    }

    bool Context::is_background_request() const
    {
        return !tokens.empty() && tokens.back() == std::string(1, BACKGROUND_SUFFIX); // copy constructor
    }

    Context Context::get_context(const std::shared_ptr<Client> &client, const std::string &message, const std::optional<CommandConstraint> &constraint)
    {
        const auto tokens = utils::split(message);
#ifdef DEBUG
        std::cout << "Parsing context from tokens: " << tokens << std::endl;
#endif

        std::map<std::string, std::vector<std::string>> values;
        std::set<std::string> present;

        if (constraint.has_value())
        {
            // Preprocess the tokens: split "-abc" into "-a", "-b", "-c" if all are valid options, etc.
            // Copy all tokens to a linked list for fast insertion/deletion.
            std::list<std::string> new_tokens(tokens.begin(), tokens.end());
            for (auto iter = new_tokens.begin(); iter != new_tokens.end(); iter++)
            {
                if (utils::startswith(*iter, "-"))
                {
                    if (utils::is_valid_long_option(*iter))
                    {
                        // pass
                    }
                    else if (iter->size() > 2)
                    {
                        // Split "-abc" into "-a", "-b", "-c" (no checking is done yet)
                        std::vector<std::string> options;
                        for (std::size_t i = 1; i < iter->size(); i++)
                        {
                            options.push_back(utils::format("-%c", iter->at(i)));
                        }

                        // Now we perform options checking
                        bool split = true;
                        for (auto &option : options)
                        {
                            if (!constraint->has_option(option))
                            {
                                split = false;
                                break;
                            }
                        }

                        if (split)
                        {
                            // All options are valid, insert the split options into the linked list
#ifdef DEBUG
                            std::cout << "Split " << *iter << " to " << options << std::endl;
#endif

                            // Delete the current token e.g. "-abc"
                            iter = new_tokens.erase(iter);

                            // Insert the split tokens e.g. "-a", "-b", "-c"
                            iter = new_tokens.insert(iter, options.begin(), options.end());
                        }
                    }
                }
            }

#ifdef DEBUG
            std::cout << "Splitted tokens: " << new_tokens << std::endl;
#endif

            // Vectorize the linked list for fast random access
            const std::vector<std::string> new_tokens_v(new_tokens.begin(), new_tokens.end());
            const auto options = constraint->get_options_map();

            auto positional_iter = constraint->positional.begin();
            std::map<std::string, std::vector<PositionalArgument>::const_iterator> options_iter;
            for (auto &[name, option] : options)
            {
                options_iter[name] = option.positional.cbegin();
            }

            for (std::size_t i = 1; i < new_tokens_v.size(); i++)
            {
                const auto token = new_tokens_v[i];
#ifdef DEBUG
                std::cout << "Parsing at i = " << i << ", token = \"" << token << "\"" << std::endl;
#endif

                const auto option_iter = options.find(token);
                if (option_iter != options.end())
                {
                    bool inserted = true;
                    for (auto &name : option_iter->second.names())
                    {
                        inserted = inserted && present.insert(name).second;
                    }
                    if (!inserted)
                    {
                        throw std::invalid_argument(utils::format("\"%s\" was specified twice", token.c_str()));
                    }

                    i++;

                    while (i < new_tokens_v.size() && !constraint->has_option(new_tokens_v[i]) && options_iter[token] != option_iter->second.positional.cend())
                    {
                        for (auto &name : option_iter->second.names())
                        {
                            auto qualified_name = name + " " + options_iter[name]->name;
                            values[qualified_name].push_back(new_tokens_v[i]);
                            present.insert(qualified_name);
                            if (!options_iter[name]->many)
                            {
                                options_iter[name]++;
                            }
                        }

                        i++;
                    }

                    if (i < new_tokens_v.size())
                    {
                        i--;
                    }
                }
                else if (utils::is_valid_short_option(token) || utils::is_valid_long_option(token))
                {
                    throw UnrecognizedOption(token);
                }
                else if (positional_iter == constraint->positional.end())
                {
                    throw TooManyPositionalArguments();
                }
                else
                {
                    values[positional_iter->name].push_back(token);
                    present.insert(positional_iter->name);
                    if (!positional_iter->many)
                    {
                        positional_iter++;
                    }
                }
            }

#ifdef DEBUG
            std::cout << "Context obtained:" << std::endl;
            std::cout << "values = " << values << std::endl;
            std::cout << "present = " << present << std::endl;
#endif

            for (auto &argument : constraint->positional)
            {
                if (argument.required && present.count(argument.name) == 0)
                {
                    throw ArgumentMissingError(argument.name);
                }
            }

            for (auto &[name, option] : options)
            {
                if (option.required || present.count(name) == 1)
                {
                    if (present.count(name) == 0)
                    {
                        throw ArgumentMissingError(name);
                    }

                    for (auto &argument : option.positional)
                    {
                        auto qualified_name = name + " " + argument.name;
                        if (argument.required && present.count(qualified_name) == 0)
                        {
                            throw ArgumentMissingError(qualified_name);
                        }
                    }
                }
            }
        }

        return Context(message, tokens, values, present, client, constraint);
    }
}
