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
            limits.resize(headers.size(), (std::size_t)-1);
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
#ifdef DEBUG
            std::cout << "Constructing table" << std::endl;
#endif
            std::vector<std::size_t> column_widths(columns);
            for (auto &row : rows)
            {
                for (std::size_t column = 0; column < columns; column++)
                {
                    column_widths[column] = std::max(column_widths[column], 2u + std::min(limits[column], row[column].size()));
                }
            }

            std::vector<std::string> lines;
            for (std::size_t row = 0; row < rows.size(); row++)
            {
                for (std::size_t lines_in_row = 0;; lines_in_row++)
                {
                    std::stringstream line;
                    bool has_content = false;
                    for (std::size_t column = 0; column < columns; column++)
                    {
                        std::string substring;
                        try
                        {
                            substring = rows[row][column].substr(lines_in_row * limits[column], limits[column]);
#ifdef DEBUG
                            std::cout << "Got substring \"" << substring << "\"" << std::endl;
#endif

                            has_content = true;
                        }
                        catch (std::out_of_range &)
                        {
                            // pass
                        }

                        if (align_left)
                        {
                            line << ' ';
                            line << substring;
                        }

                        for (std::size_t i = 0; i < column_widths[column] - substring.size() - 1; i++)
                        {
                            line << ' ';
                        }

                        if (!align_left)
                        {
                            line << substring;
                            line << ' ';
                        }

                        line << '|';
                    }

                    if (has_content)
                    {
#ifdef DEBUG
                        std::cout << "Got line \"" << line.str() << "\"" << std::endl;
#endif

                        lines.push_back(line.str());
                    }
                    else
                    {
                        break;
                    }
                }

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

#ifdef DEBUG
            std::cout << "Constructed table" << std::endl;
#endif

            return result;
        }
    };
}