#pragma once

#include "standard.hpp"

class InputStream
{
private:
    std::deque<std::string> queue;

public:
    bool echo = true;

    bool handle(const std::string &line)
    {
        if (line == "@ON")
        {
            echo = true;
            return true;
        }

        if (line == "@OFF")
        {
            echo = false;
            return true;
        }

        return false;
    }

    bool peek_echo()
    {
        if (peek() == "@ON")
        {
            return true;
        }

        if (peek() == "@OFF")
        {
            return false;
        }

        return echo;
    }

    std::optional<std::string> peek()
    {
        if (queue.empty())
        {
            return std::nullopt;
        }

        return queue.front();
    }

    std::string getline(const bool force_stdin = false, const bool force_stream = false)
    {
        if (force_stdin && force_stream)
        {
            throw std::invalid_argument("Arguments conflict: force_stdin && force_stream");
        }

        // std::cout << "(force_stream = " << force_stream << ", queue.size() = " << queue.size() << ") ";
        if (force_stream && queue.empty())
        {
            throw std::runtime_error("Unexpected EOF while reading");
        }

        if (force_stdin || queue.empty())
        {
            // std::cout << "(waiting for stdin, queue size = " << queue.size() << ") ";
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

                if (handle(line))
                {
                    return getline(force_stdin, force_stream);
                }

                return line;
            }
        }
        else
        {
            auto line = strip(queue.front());
            queue.pop_front();

            if (handle(line))
            {
                return getline(force_stdin, force_stream);
            }

            if (echo)
            {
                std::cout << line << std::endl;
            }

            return line;
        }
    }

    void write_front(const std::string &data)
    {
        for (auto &line : split(data, '\n'))
        {
            if (!line.empty())
            {
                queue.push_front(line);
            }
        }
    }

    void write_back(const std::string &data)
    {
        for (auto &line : split(data, '\n'))
        {
            if (!line.empty())
            {
                queue.push_back(line);
            }
        }
    }

    bool eof() const
    {
        return queue.empty();
    }
};