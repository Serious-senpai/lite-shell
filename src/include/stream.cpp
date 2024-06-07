#include "stream.hpp"

namespace liteshell
{
    InputStream::InputStream() {}

    bool InputStream::peek_echo()
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

    std::optional<std::string> InputStream::peek()
    {
        for (auto iter = _iterator; iter != _list.end(); iter++)
        {
            auto text = utils::strip(*iter);
            if (!text.empty())
            {
                return text;
            }
        }

        return std::nullopt;
    }

    std::string InputStream::getline(const std::string &prompt, const int flags)
    {
        if ((flags & FORCE_STDIN) & (flags & FORCE_STREAM))
        {
            throw std::invalid_argument("Arguments conflict: FORCE_STDIN && FORCE_STREAM");
        }

        if ((flags & FORCE_STREAM) && eof())
        {
            throw std::runtime_error("Unexpected EOF while reading");
        }

        if ((flags & FORCE_STDIN) || eof())
        {
            while (true)
            {
                if ((flags & FORCE_STDOUT) || (echo && peek_echo()))
                {
                    std::cout << prompt << std::flush;
                }

                std::string line;
                std::getline(std::cin, line);

                line = utils::strip(line);
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
            auto line = utils::strip(*_iterator++);
            if (echo_state)
            {
                std::cout << prompt << line << std::endl;
            }

            return line;
        }
    }

    template <typename _ForwardIterator>
    void InputStream::write(const _ForwardIterator &__begin, const _ForwardIterator &__end)
    {
        _iterator = _list.insert(_iterator, __begin, __end);
    }

    void InputStream::write(const std::string &data)
    {
        std::vector<std::string> lines;
        for (auto &line : utils::split(data, '\n'))
        {
            line = utils::strip(line);
            if (!line.empty())
            {
                lines.push_back(line);
            }
        }

        write(lines.begin(), lines.end());
    }

    bool InputStream::eof() const
    {
        return _iterator == _list.end();
    }

    void InputStream::jump(const std::string &label)
    {
        for (auto iter = _iterator; iter != _list.end(); iter++)
        {
            if (utils::strip(*iter) == label)
            {
                _iterator = iter;
                return;
            }
        }

        for (auto iter = _list.begin(); iter != _iterator; iter++)
        {
            if (utils::strip(*iter) == label)
            {
                _iterator = iter;
                return;
            }
        }

        throw std::runtime_error(utils::format("Label %s not found", label.c_str()));
    }
}
