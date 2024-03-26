#pragma once

#include "standard.hpp"
#include "split.hpp"
#include "strip.hpp"

#define ECHO_OFF "@OFF"
#define ECHO_ON "@ON"

class InputStream
{
private:
    std::list<std::string> list;
    std::list<std::string>::iterator iterator = list.begin();

public:
    static const int FORCE_STDOUT = 0b100;
    static const int FORCE_STDIN = 0b010;
    static const int FORCE_STREAM = 0b001;

    bool echo = true;

    bool peek_echo()
    {
        if (peek() == ECHO_ON)
        {
            return true;
        }

        if (peek() == ECHO_OFF)
        {
            return false;
        }

        return echo;
    }

    std::optional<std::string> peek()
    {
        for (auto iter = iterator; iter != list.end(); iter++)
        {
            auto text = strip(*iter);
            if (!text.empty())
            {
                return text;
            }
        }

        return std::nullopt;
    }

    std::string getline(const std::string &prompt, const int flags)
    {
        if ((flags & FORCE_STDIN) & (flags & FORCE_STREAM))
        {
            throw std::invalid_argument("Arguments conflict: force_stdin && force_stream");
        }

        if ((flags & FORCE_STREAM) && iterator == list.end())
        {
            throw std::runtime_error("Unexpected EOF while reading");
        }

        if ((flags & FORCE_STDIN) || iterator == list.end())
        {
            while (true)
            {
                if ((flags & FORCE_STDOUT) || (echo && peek_echo()))
                {
                    std::cout << prompt;
                    std::cout.flush();
                }

                std::string line;
                std::getline(std::cin, line);

                line = strip(line);
                if (std::cin.fail() || std::cin.eof() || line.empty())
                {
                    std::cin.clear();
                    std::cout << std::endl;
                    continue;
                }

                return line;
            }
        }
        else
        {
            auto echo_state = echo && peek_echo();
            auto line = strip(*iterator++);
            if (echo_state)
            {
                std::cout << prompt << line << std::endl;
            }

            return line;
        }
    }

    template <typename _ForwardIterator>
    void write(const _ForwardIterator &__begin, const _ForwardIterator &__end)
    {
        iterator = list.insert(iterator, __begin, __end);
    }

    void write(const std::string &data)
    {
        std::vector<std::string> lines;
        for (auto &line : split(data, '\n'))
        {
            line = strip(line);
            if (!line.empty())
            {
                lines.push_back(line);
            }
        }

        write(lines.begin(), lines.end());
    }

    bool eof() const
    {
        return iterator == list.end();
    }

    void jump(const std::string &label)
    {
        for (auto iter = iterator; iter != list.end(); iter++)
        {
            if (strip(*iter) == label)
            {
                iterator = iter;
                return;
            }
        }

        for (auto iter = list.begin(); iter != iterator; iter++)
        {
            if (strip(*iter) == label)
            {
                iterator = iter;
                return;
            }
        }

        throw std::runtime_error(format("Label %s not found", label.c_str()));
    }
};
