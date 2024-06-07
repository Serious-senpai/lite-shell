#include "utils.hpp"

namespace utils
{
    std::string last_error(const std::string &message)
    {
        return format("%s: %d", message.c_str(), GetLastError());
    }

    std::pair<SHORT, SHORT> get_console_size()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        {
            throw std::runtime_error(last_error("GetConsoleScreenBufferInfo ERROR"));
        }

        SHORT columns = info.srWindow.Right - info.srWindow.Left + 1,
              rows = info.srWindow.Bottom - info.srWindow.Top + 1;

        return std::make_pair(columns, rows);
    }

    std::string get_working_directory()
    {
        wchar_t buffer[MAX_PATH];
        auto size = GetCurrentDirectoryW(MAX_PATH, buffer);
        if (size == 0)
        {
            throw std::runtime_error(last_error("GetCurrentDirectoryW ERROR"));
        }

        return utf_convert(std::wstring(buffer, buffer + size));
    }

    std::string get_executable_path()
    {
        wchar_t buffer[MAX_PATH];
        auto size = GetModuleFileNameW(NULL, buffer, MAX_PATH);
        if (size == 0)
        {
            throw std::runtime_error(last_error("Error calling GetModuleFileNameW"));
        }

        return utf_convert(std::wstring(buffer, buffer + size));
    }

    BOOL WINAPI ctrl_handler(DWORD ctrl_type)
    {
        return ctrl_type == CTRL_C_EVENT;
    }

    void set_ignore_ctrl_c(bool ignore)
    {
        if (!SetConsoleCtrlHandler(ctrl_handler, ignore))
        {
            std::cerr << last_error("Warning: SetConsoleCtrlHandler ERROR") << std::endl;
        }
    }

    std::vector<WIN32_FIND_DATAW> list_files(const std::string &__pattern)
    {
        std::vector<WIN32_FIND_DATAW> results(1);

        HANDLE h_file = FindFirstFileW(utf_convert(__pattern).c_str(), &results[0]);
        if (h_file == INVALID_HANDLE_VALUE)
        {
            return {};
        }

        do
        {
            results.emplace_back();
        } while (FindNextFileW(h_file, &results.back()));

        results.pop_back();

        if (!FindClose(h_file))
        {
            throw std::runtime_error(last_error("Error when closing file search handle"));
        }

        CloseHandle(h_file);
        return results;
    }

    bool remove_directory(const std::string &directory, bool verbose)
    {
        bool success = true;
        for (const auto &file : list_files(join(directory, "*")))
        {
            auto filename = utf_convert(file.cFileName);

            // Skip current and parent directory
            if (filename == "." || filename == "..")
            {
                continue;
            }

            auto path = join(directory, filename);
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!remove_directory(path, verbose))
                {
                    success = false;
                }
            }
            else if (DeleteFileW(utf_convert(path).c_str()))
            {
                std::cout << "Deleted " << path << std::endl;
            }
            else
            {
                std::cerr << last_error(format("Error deleting file \"%s\"", path.c_str())) << std::endl;
                success = false;
            }
        }

        if (RemoveDirectoryW(utf_convert(directory).c_str()))
        {
            std::cout << "Deleted " << directory << std::endl;
        }
        else
        {
            std::cerr << last_error(format("Error deleting directory \"%s\"", directory.c_str())) << std::endl;
            success = false;
        }

        return success;
    }

    bool startswith(const std::string &string, const std::string &prefix)
    {
        if (string.size() < prefix.size())
        {
            return false;
        }

        for (std::size_t i = 0; i < prefix.size(); i++)
        {
            if (string[i] != prefix[i])
            {
                return false;
            }
        }

        return true;
    }

    bool endswith(const std::string &string, const std::string &suffix)
    {
        if (string.size() < suffix.size())
        {
            return false;
        }

        for (std::size_t i = string.size() - suffix.size(); i < string.size(); i++)
        {
            if (string[i] != suffix[i + suffix.size() - string.size()])
            {
                return false;
            }
        }

        return true;
    }

    std::string regex_escape(const std::string &original)
    {
        std::stringstream result;
        for (char c : original)
        {
            if (c == '.' || c == '*' || c == '+' || c == '?' || c == '|' || c == '(' || c == ')' ||
                c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '^' || c == '$')
            {
                result << '\\';
            }
            result << c;
        }
        return result.str();
    }

    bool is_executable(LPCWSTR name)
    {
        DWORD _;
        return GetBinaryTypeW(name, &_);
    }

    std::string ngettext(const bool predicate, const std::string &first, const std::string &second)
    {
        return predicate ? first : second;
    }

    bool is_math_symbol(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == ' ' || c == '%' || ('0' <= c && c <= '9') || c == '(' || c == ')';
    }

    bool is_valid_command(const std::string &name)
    {
        return boost::regex_match(name, _command_name);
    }

    bool is_valid_variable(const std::string &name)
    {
        return boost::regex_match(name, _variable_name);
    }

    bool is_valid_short_option(const std::string &name)
    {
        return boost::regex_match(name, _short_option_name);
    }

    bool is_valid_long_option(const std::string &name)
    {
        return boost::regex_match(name, _long_option_name);
    }

    MEMORYSTATUSEX get_global_memory_status()
    {
        MEMORYSTATUSEX memory_status;
        memory_status.dwLength = sizeof(memory_status);
        if (!GlobalMemoryStatusEx(&memory_status))
        {
            throw std::runtime_error(last_error("GlobalMemoryStatusEx ERROR"));
        }

        return memory_status;
    }

    template <typename T>
    T sqrt(const T &value)
    {
        if (value < 0)
        {
            throw std::out_of_range(format("Attempted to calculate square root of %s < 0", std::to_string(value).c_str()));
        }

        if (value == 0)
        {
            return 0;
        }

        T low = 0, high = std::max(static_cast<T>(1), value), accuracy = 1;
        if constexpr (std::is_floating_point_v<T>)
        {
            accuracy = 1.0e-6;
        }

        while (high - low > accuracy)
        {
            double mid = (low + high) / 2;
            if (mid * mid < value)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
        }

        return high;
    }

    Finalize::Finalize(const std::function<void()> &callback) : callback(callback) {}
    Finalize::~Finalize()
    {
        callback();
    }
}
