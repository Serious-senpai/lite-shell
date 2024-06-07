#pragma once

#include "format.hpp"
#include "split.hpp"
#include "strip.hpp"

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
        InputStream();

        /** @brief The echo state after the next command */
        bool peek_echo();

        /**
         * @brief Peek the next command in the stream.
         * @return The next command in the input stream, or `std::nullopt` if the stream
         * reaches EOF
         */
        std::optional<std::string> peek();

        /**
         * @brief Read the next command
         *
         * @param prompt The prompt to display before reading
         * @param flags The flags to use when reading the command
         * @return The next command in the input stream
         */
        std::string getline(const std::string &prompt, const int flags);

        template <typename _ForwardIterator>
        void write(const _ForwardIterator &__begin, const _ForwardIterator &__end);

        void write(const std::string &data);

        /** @brief Whether this stream reaches EOF */
        bool eof() const;

        /** @brief Jump to the specified label */
        void jump(const std::string &label);
    };
}
