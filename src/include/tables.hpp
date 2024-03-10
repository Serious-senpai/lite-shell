#pragma once

#include "format.hpp"
#include "standard.hpp"

class Table
{
private:
    std::vector<std::vector<std::string>> rows;

public:
    const unsigned columns;
    bool align_left = true;

    Table(const std::initializer_list<std::string> &header) : columns(header.size())
    {
        rows.push_back(header);
    }

    void add_row(const std::initializer_list<std::string> &row)
    {
        if (row.size() != columns)
        {
            throw std::invalid_argument(format("Attempted to add a row of %d item(s) to a table with only %d column(s)", row.size(), columns));
        }

        rows.push_back(row);
    }

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