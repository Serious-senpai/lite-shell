#include "tables.hpp"

namespace utils
{
    Table::Table(const std::initializer_list<std::string> &headers) : columns(headers.size())
    {
        rows.push_back(headers);
        limits.resize(headers.size(), (std::size_t)-1);
    }

    void Table::add_row(const std::initializer_list<std::string> &row)
    {
        if (row.size() != columns)
        {
            throw std::invalid_argument(format("Attempted to add a row of %d item(s) to a table with %d column(s)", row.size(), columns));
        }

        rows.push_back(row);
    }

    template <typename... Args>
    Table::Table(const Args &...columns) : Table({columns...}) {}

    template <typename... Args>
    void Table::add_row(const Args &...values)
    {
        add_row({values...});
    }

    std::string Table::display()
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
}
