#pragma once

#include "standard.hpp"

#define ECHO_OFF "@OFF"
#define ECHO_ON "@ON"

class InputStream
{
private:
    std::list<std::string> list;
    std::list<std::string>::iterator iterator = list.begin();

public:
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

    std::string getline(const bool force_stdin = false, const bool force_stream = false)
    {
        if (force_stdin && force_stream)
        {
            throw std::invalid_argument("Arguments conflict: force_stdin && force_stream");
        }

        if (force_stream && iterator == list.end())
        {
            throw std::runtime_error("Unexpected EOF while reading");
        }

        if (force_stdin || iterator == list.end())
        {
            while (true)
            {
                std::string line;
                std::getline(std::cin, line);

                line = strip(line);
                if (std::cin.fail() || std::cin.eof() || line.empty())
                {
                    std::cin.clear();
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
                std::cout << line << std::endl;
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
        while (iterator != list.end())
        {
            auto next = *iterator++;

            if (next == label)
            {
                return;
            }
        }

        throw std::runtime_error(format("Label %s not found", label.c_str()));
    }
};
