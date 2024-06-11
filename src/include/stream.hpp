#pragma once

#include "standard.hpp"

namespace liteshell
{
    /**
     * @brief The input stream manager
     *
     * This class manages the input stream for the shell. Typically, the input comes from stdin, but when reading from
     * a batch script, the input stream comes from the script file instead.
     */
    class InputStream
    {
    private:
        /**
         * @brief A special command to turn off echo.
         *
         * This is handled by the `InputStream::getline`
         */
        static const std::string ECHO_OFF;

        /**
         * @brief A special command to turn on echo.
         *
         * This is handled by the `InputStream::getline`
         */
        static const std::string ECHO_ON;

        /**
         * @brief A special label appended at the end of each batch script.
         */
        static const std::string STREAM_EOF;

        std::list<std::string> _list;

        /** @brief Mimic the [instruction pointer](https://en.wikipedia.org/wiki/Program_counter) */
        std::list<std::string>::iterator _iterator = _list.begin();

        InputStream(const InputStream &) = delete;
        InputStream &operator=(const InputStream &) = delete;

        /** @brief The current echo state */
        bool _echo = true;

    public:
        /** @brief A flag indicating that `getline` must echo the input to stdout */
        static const int FORCE_ECHO = 1 << 0;

        /** @brief A flag indicating that `getline` must read input from stdin */
        static const int FORCE_STDIN = 1 << 1;

        /** @brief A flag indicating that `getline` must read input from file */
        static const int FORCE_STREAM = 1 << 2;

        /**
         * @brief Construct a new `InputStream` object
         */
        InputStream() {}

        /** @brief The echo state after the next command */
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

            return _echo;
        }

        /**
         * @brief Peek the next command in the stream.
         *
         * The search starts from the underlying intruction pointer
         *
         * @return The next command in the input stream, or `std::nullopt` if the stream
         * reaches EOF
         */
        std::optional<std::string> peek()
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

        /**
         * @brief Read the next command
         *
         * @param prompt The function to display the prompt string before reading
         * @param flags The flags to use when reading the command
         * @return The next command in the input stream
         */
        std::string getline(const std::function<void()> &prompt, const int flags)
        {
#ifdef DEBUG
            std::cout << "Received getline request, flags = " << flags << std::endl;
            std::cout << "Current input stream: " << std::endl;
            for (const auto &line : _list)
            {
                std::cout << line << std::endl;
            }
            std::cout << "Iterator position: " << std::distance(_list.begin(), _iterator);
            if (_iterator != _list.end())
            {
                std::cout << " (\"" << *_iterator << "\")";
            }
            std::cout << std::endl;
#endif

            if ((flags & FORCE_STDIN) & (flags & FORCE_STREAM))
            {
                throw std::invalid_argument("Arguments conflict: FORCE_STDIN && FORCE_STREAM");
            }

            if ((flags & FORCE_STREAM) && exhaust())
            {
                throw std::runtime_error("Unexpected EOF while reading");
            }

            if ((flags & FORCE_ECHO) || (_echo && peek_echo()))
            {
                prompt();
            }

            if (exhaust())
            {
                _list.clear();
                _iterator = _list.begin();
            }

            bool from_stdin = (flags & FORCE_STDIN) || exhaust();

            std::string line;
            if (from_stdin)
            {
                std::getline(std::cin, line);
                if (std::cin.fail() || std::cin.eof())
                {
                    std::cin.clear();
                    std::cout << std::endl;
                    return getline(prompt, flags);
                }
            }
            else
            {
                line = *_iterator++;
            }
            line = utils::strip(line);

            if (line == ECHO_OFF)
            {
                _echo = false;
                return getline(prompt, flags);
            }
            else if (line == ECHO_ON)
            {
                _echo = true;
                return getline(prompt, flags);
            }

            if (line == STREAM_EOF)
            {
                if (flags & FORCE_STREAM)
                {
                    throw std::runtime_error("Unexpected EOF while reading");
                }
            }

            if (!from_stdin && _echo)
            {
                std::cout << line << std::endl;
            }

#ifdef DEBUG
            std::cout << "Response for getline request: " << line << std::endl;
#endif
            return line;
        }

        void consume_last()
        {
            auto iter = _iterator;
            if (iter != _list.begin())
            {
                _list.erase(--iter);
            }
        }

        /**
         * @brief Insert commands after the current instruction pointer
         *
         * @param __begin A forward iterator pointing to the beginning of the input
         * @param __end A forward iterator pointing past the end of the input
         */
        template <typename _ForwardIterator>
        void write(const _ForwardIterator &__begin, const _ForwardIterator &__end)
        {
            _iterator = _list.insert(_iterator, __begin, __end);
        }

        void write(const std::string &data)
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

        /** @brief Whether the underlying iterator reaches the end of the linked-list */
        bool exhaust() const
        {
            return _iterator == _list.end();
        }

        void append_footer(std::stringstream &stream)
        {
            stream << "\n";
            stream << STREAM_EOF << "\n";
            stream << (_echo ? ECHO_ON : ECHO_OFF) << "\n";
        }

        /** @brief Jump to the specified label */
        void jump(const std::string &label)
        {
#ifdef DEBUG
            std::cout << "Jumping to label " << label << std::endl;
#endif

            if (_list.empty())
            {
                throw std::runtime_error("Cannot jump to the specified label since the input stream is empty");
            }

            if (_iterator == _list.end())
            {
                _iterator = _list.begin();
            }

            auto _iterator_old = _iterator;
            while (utils::strip(*_iterator) != label)
            {
                _iterator++;
                if (_iterator == _list.end())
                {
                    _iterator = _list.begin();
                }
                if (_iterator == _iterator_old)
                {
                    throw std::runtime_error(utils::format("Label \"%s\" not found", label.c_str()));
                }
            }
        }
    };

    const std::string InputStream::ECHO_ON = "@ON";
    const std::string InputStream::ECHO_OFF = "@OFF";
    const std::string InputStream::STREAM_EOF = ":EOF";
}
