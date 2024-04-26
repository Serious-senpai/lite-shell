#include <all.hpp>

int main(int argc, const char **argv)
{
    auto directory = utils::get_working_directory();
    if (argc > 1)
    {
        directory = argv[1];
    }

    std::cout << "Content of " << directory << ":" << std::endl;

    std::vector<std::pair<int, std::pair<std::string, WIN32_FIND_DATAW>>> stack;
    for (const auto &child : utils::explore_directory(directory))
    {
        stack.push_back(std::make_pair(0, std::make_pair(directory, child)));
    }

    while (!stack.empty())
    {
        const auto [level, file] = stack.back();
        const auto [path, data] = file;

        stack.pop_back();

        for (int i = 0; i < level - 1; i++)
        {
            std::cout << (char)179 << "   "; // │
        }

        if (stack.empty() || stack.back().first < level)
        {
            std::cout << (char)192; // └
        }
        else
        {
            std::cout << (char)195; // ├
        }
        std::cout << (char)196 << (char)196 << (char)196; // ───

        auto filename = utils::utf_convert(data.cFileName);
        std::cout << filename << '\n';

        if (
            (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
            !(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
            (filename != ".") &&
            (filename != ".."))
        {
            const auto new_path = utils::join(path, filename);
            for (const auto &child : utils::explore_directory(new_path))
            {
                stack.push_back(std::make_pair(level + 1, std::make_pair(new_path, child)));
            }
        }
    }

    return 0;
}
