#pragma once

#include "format.hpp"

namespace utils
{
    /** @brief Helper class to display ASCII table on the console */
    class Table
    {
    private:
        std::vector<std::vector<std::string>> rows;

        Table(const std::initializer_list<std::string> &headers) : columns(headers.size())
        {
            rows.push_back(headers);
        }

        void add_row(const std::initializer_list<std::string> &row)
        {
            if (row.size() != columns)
            {
                throw std::invalid_argument(format("Attempted to add a row of %d item(s) to a table with %d column(s)", row.size(), columns));
            }

            rows.push_back(row);
        }

    public:
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
        Table(const Args &...columns) : Table({columns...}) {}

        /**
         * @brief Add a new row to the table.
         *
         * The number of columns in this row must match the number of columns in the table.
         *
         * @param values The values in the row
         */
        template <typename... Args>
        void add_row(const Args &...values)
        {
            add_row({values...});
        }

        /** @brief An ASCII string displaying the table */
        std::string display()
        {
            std::vector<std::size_t> column_widths(columns);
            for (auto &row : rows)
            {
                for (std::size_t column = 0; column < columns; column++)
                {
                    column_widths[column] = std::max(column_widths[column], 2u + row[column].size());
                }
            }

            std::vector<std::string> lines;
            for (std::size_t row = 0; row < rows.size(); row++)
            {
                std::string line;
                for (std::size_t column = 0; column < columns; column++)
                {
                    if (align_left)
                    {
                        line += ' ';
                        line += rows[row][column];
                    }

                    for (std::size_t i = 0; i < column_widths[column] - rows[row][column].size() - 1; i++)
                    {
                        line += ' ';
                    }

                    if (!align_left)
                    {
                        line += rows[row][column];
                        line += ' ';
                    }

                    line += '|';
                }

                lines.push_back(line);

                if (row == 0)
                {
                    std::string line;
                    for (std::size_t column = 0; column < columns; column++)
                    {
                        for (std::size_t i = 0; i < column_widths[column]; i++)
                        {
                            line += '-';
                        }
                        line += '+';
                    }
                    lines.push_back(line);
                }
            }

            std::string result;
            for (auto &line : lines)
            {
                result += line;
                result += '\n';
            }

            return result;
        }
    };
}