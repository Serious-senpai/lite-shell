#pragma once

#include "format.hpp"

/* A helper class to display ASCII table on the console */
class Table
{
private:
    std::vector<std::vector<std::string>> rows;

public:
    /* The number of columns, this number is unchanged */
    const unsigned columns;

    /* Whether each row should align left */
    bool align_left = true;

    /* Construct a new table with the specified headers */
    Table(const std::initializer_list<std::string> &headers) : columns(headers.size())
    {
        rows.push_back(headers);
    }

    /**
     * Add a new row to the table.
     *
     * @param row The row to add. The number of items in the row must be equal to the number of table columns.
     */
    void add_row(const std::initializer_list<std::string> &row)
    {
        if (row.size() != columns)
        {
            throw std::invalid_argument(format("Attempted to add a row of %d item(s) to a table with only %d column(s)", row.size(), columns));
        }

        rows.push_back(row);
    }

    /* An ASCII string displaying the table */
    std::string display()
    {
        std::vector<unsigned> column_widths(columns);
        for (auto &row : rows)
        {
            for (unsigned column = 0; column < columns; column++)
            {
                column_widths[column] = std::max(column_widths[column], 2u + (unsigned)row[column].size());
            }
        }

        std::vector<std::string> lines;
        for (unsigned row = 0; row < rows.size(); row++)
        {
            std::string line;
            for (unsigned column = 0; column < columns; column++)
            {
                if (align_left)
                {
                    line += ' ';
                    line += rows[row][column];
                }

                for (unsigned i = 0; i < column_widths[column] - rows[row][column].size() - 1; i++)
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
                for (unsigned column = 0; column < columns; column++)
                {
                    for (unsigned i = 0; i < column_widths[column]; i++)
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