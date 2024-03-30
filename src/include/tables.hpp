#pragma once

#include "format.hpp"

/* A helper class to display ASCII table on the console */
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
    /* The number of columns, this number is unchanged */
    const unsigned columns;

    /* Whether each row should align left */
    bool align_left = true;

    Table(const std::string &column) : Table({column}) {}
    Table(
        const std::string &column_1,
        const std::string &column_2)
        : Table({column_1, column_2}) {}
    Table(
        const std::string &column_1,
        const std::string &column_2,
        const std::string &column_3)
        : Table({column_1, column_2, column_3}) {}
    Table(
        const std::string &column_1,
        const std::string &column_2,
        const std::string &column_3,
        const std::string &column_4)
        : Table({column_1, column_2, column_3, column_4}) {}
    Table(
        const std::string &column_1,
        const std::string &column_2,
        const std::string &column_3,
        const std::string &column_4,
        const std::string &column_5)
        : Table({column_1, column_2, column_3, column_4, column_5}) {}

    void add_row(const std::string &column)
    {
        add_row({column});
    }

    void add_row(
        const std::string &column_1,
        const std::string &column_2)
    {
        add_row({column_1, column_2});
    }

    void add_row(
        const std::string &column_1,
        const std::string &column_2,
        const std::string &column_3)
    {
        add_row({column_1, column_2, column_3});
    }

    void add_row(
        const std::string &column_1,
        const std::string &column_2,
        const std::string &column_3,
        const std::string &column_4)
    {
        add_row({column_1, column_2, column_3, column_4});
    }

    void add_row(
        const std::string &column_1,
        const std::string &column_2,
        const std::string &column_3,
        const std::string &column_4,
        const std::string &column_5)
    {
        add_row({column_1, column_2, column_3, column_4, column_5});
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