#pragma once

#define ECHO_OFF "@OFF"
#define ECHO_ON "@ON"

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
        std::list<std::string> _list;
        std::list<std::string>::iterator _iterator = _list.begin();

        InputStream(const InputStream &) = delete;
        InputStream &operator=(const InputStream &) = delete;

    public:
        /** @brief A flag indicating that `getline` must echo the input to stdout */
        static const int FORCE_STDOUT = 1 << 2;

        /** @brief A flag indicating that `getline` must read input from stdin */
        static const int FORCE_STDIN = 1 << 1;

        /** @brief A flag indicating that `getline` must read input from file */
        static const int FORCE_STREAM = 1 << 0;

        /** @brief The current echo state */
        bool echo = true;

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

            return echo;
        }

        /**
         * @brief Peek the next command in the stream.
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
         * @param prompt The prompt to display before reading
         * @param flags The flags to use when reading the command
         * @return The next command in the input stream
         */
        std::string getline(const std::string &prompt, const int flags)
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

        /** @brief Whether this stream reaches EOF */
        bool eof() const
        {
            return _iterator == _list.end();
        }

        /** @brief Jump to the specified label */
        void jump(const std::string &label)
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
    };
}
