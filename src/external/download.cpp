#include <all.hpp>

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        throw std::invalid_argument("Expected a download URL and a local path");
    }

    const auto url = std::string(argv[1]);
    const auto path = std::string(argv[2]);
    const auto wurl = utils::utf_convert(url);

    const auto component = utils::URL::parse(url);
    if (component.scheme != "http" && component.scheme != "https")
    {
        throw std::invalid_argument("Only HTTP and HTTPS are supported");
    }

    std::cout << "Scheme: " << component.scheme << std::endl;
    std::cout << "Hostname: " << component.hostname << std::endl;
    std::cout << "Username: " << component.username << std::endl;
    std::cout << "Password: " << component.password << std::endl;
    std::cout << "Port: " << component.port << std::endl;
    std::cout << "Path: " << component.path << std::endl;
    std::cout << "Extra info: " << component.extra_info << std::endl;

    std::vector<std::unique_ptr<utils::Finalize>> finalizers;

    auto session = InternetOpenW(
        L"Serious-senpai/lite-shell",
        INTERNET_OPEN_TYPE_DIRECT,
        NULL,
        NULL,
        0);
    if (session == NULL)
    {
        throw std::runtime_error(utils::last_error("wininet.h InternetOpenW ERROR"));
    }
    finalizers.push_back(
        std::make_unique<utils::Finalize>(
            [&session]()
            {
                InternetCloseHandle(session);
                std::cout << "Closed session" << std::endl;
            }));

    auto request = InternetOpenUrlW(
        session,      // hInternet
        wurl.c_str(), // lpszUrl
        NULL,         // lpszHeaders
        0,            // dwHeadersLength
        0,            // dwFlags
        0             // dwContext
    );
    if (request == NULL)
    {
        throw std::runtime_error(utils::last_error("wininet.h InternetOpenUrlW ERROR"));
    }
    finalizers.push_back(
        std::make_unique<utils::Finalize>(
            [&request]()
            {
                InternetCloseHandle(request);
                std::cout << "Closed request" << std::endl;
            }));

    auto file = CreateFileW(
        utils::utf_convert(path).c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error(utils::last_error("CreateFileW ERROR"));
    }

    char buffer[LITE_SHELL_BUFFER_SIZE];
    DWORD write = 0, total = 0;
    std::cout << "Downloading from \"" << url << "\" to \"" << path << "\"" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    do
    {
        if (!InternetReadFile(request, buffer, LITE_SHELL_BUFFER_SIZE, &write))
        {
            throw std::runtime_error(utils::last_error("wininet.h InternetReadFile ERROR"));
        }

        if (write > 0)
        {
            WriteFile(file, buffer, write, NULL, NULL);
            total += write;
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Downloaded: " << utils::memory_size(total);

        auto duration = static_cast<long double>(std::chrono::duration_cast<std::chrono::seconds>(end - start).count());
        if (duration > 0)
        {
            std::cout << " (" << utils::memory_size(static_cast<long double>(total) / duration) << "/s)";
        }
        std::cout << "          \r" << std::flush;

    } while (write > 0);
    std::cout << std::endl;

    return 0;
}
