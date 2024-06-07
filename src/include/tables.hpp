#pragma once

#include "format.hpp"

namespace utils
{
    /** @brief Helper class to display ASCII table on the console */
    class Table
    {
    private:
        std::vector<std::vector<std::string>> rows;

        Table(const std::initializer_list<std::string> &headers);
        void add_row(const std::initializer_list<std::string> &row);

    public:
        /** @brief The characters limit for each row. This limit is set to -1 by default (the largest unsigned value). */
        std::vector<std::size_t> limits;

        /** @brief The number of columns in the table */
        const std::size_t columns;

        /** @brief Whether each row should align left or right */
        bool align_left = true;

        /**
         * @brief Construct a new `Table` with the specified column headers.
         *
         * The number of column headers will be equal to the number of columns in the table.
         *
         * @see `columns`
         * @param columns The column headers
         */
        template <typename... Args>
        Table(const Args &...columns);

        /**
         * @brief Add a new row to the table.
         *
         * The number of columns in this row must match the number of columns in the table.
         *
         * @param values The values in the row
         */
        template <typename... Args>
        void add_row(const Args &...values);

        /** @brief An ASCII string displaying the table */
        std::string display();
    };
}
