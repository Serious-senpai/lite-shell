#include <all.hpp>

// Header-only, should be linked to wininet.dll at runtime
#include <wininet.h>

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        throw std::invalid_argument("Expected a download URL and a local path");
    }

    const auto url = std::string(argv[1]);
    const auto path = std::string(argv[2]);
    const auto wurl = utils::utf_convert(url);

    const auto module = LoadLibraryW(L"wininet.dll");
    if (module == NULL)
    {
        throw std::runtime_error(utils::last_error("LoadLibraryW (wininet.dll) ERROR"));
    }

    const auto _InternetCrackUrlW = (WINBOOL(*)(LPCWSTR, DWORD, DWORD, LPURL_COMPONENTSW))GetProcAddress(module, "InternetCrackUrlW");
    const auto _InternetOpenW = (HINTERNET(*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD))GetProcAddress(module, "InternetOpenW");
    // const auto _InternetConnectW = (HINTERNET(*)(HINTERNET, LPCWSTR, INTERNET_PORT, LPCWSTR, LPCWSTR, DWORD, DWORD, DWORD_PTR))GetProcAddress(module, "InternetConnectW");
    const auto _InternetOpenUrlW = (HINTERNET(*)(HINTERNET, LPCWSTR, LPCWSTR, DWORD, DWORD, DWORD_PTR))GetProcAddress(module, "InternetOpenUrlW");
    // const auto _HttpOpenRequestW = (HINTERNET(*)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR *, DWORD, DWORD_PTR))GetProcAddress(module, "HttpOpenRequestW");
    // const auto _HttpSendRequestW = (WINBOOL(*)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD))GetProcAddress(module, "HttpSendRequestW");
    const auto _InternetReadFile = (WINBOOL(*)(HINTERNET, LPVOID, DWORD, LPDWORD))GetProcAddress(module, "InternetReadFile");
    const auto _InternetCloseHandle = (WINBOOL(*)(HINTERNET))GetProcAddress(module, "InternetCloseHandle");

    URL_COMPONENTSW components;
    ZeroMemory(&components, sizeof(components));
    components.dwStructSize = sizeof(components);
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUserNameLength = (DWORD)-1;
    components.dwPasswordLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    components.dwExtraInfoLength = (DWORD)-1;

    if (!_InternetCrackUrlW(wurl.c_str(), wurl.size(), 0, &components))
    {
        throw std::runtime_error(utils::last_error("wininet.h InternetCrackUrlW ERROR"));
    }

    const auto scheme = utils::utf_convert(std::wstring(components.lpszScheme, components.dwSchemeLength));
    if (scheme != "http" && scheme != "https")
    {
        throw std::invalid_argument("Only HTTP and HTTPS are supported");
    }

    const auto hostname = utils::utf_convert(std::wstring(components.lpszHostName, components.dwHostNameLength));
    const auto username = utils::utf_convert(std::wstring(components.lpszUserName, components.dwUserNameLength));
    const auto password = utils::utf_convert(std::wstring(components.lpszPassword, components.dwPasswordLength));
    const auto urlpath = utils::utf_convert(std::wstring(components.lpszUrlPath, components.dwUrlPathLength));
    const auto extrainfo = utils::utf_convert(std::wstring(components.lpszExtraInfo, components.dwExtraInfoLength));

    std::cout << "Scheme: " << scheme << std::endl;
    std::cout << "Hostname: " << hostname << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Password: " << password << std::endl;
    std::cout << "Port: " << components.nPort << std::endl;
    std::cout << "Path: " << urlpath << std::endl;
    std::cout << "Extra info: " << extrainfo << std::endl;

    std::vector<std::unique_ptr<utils::Finalize>> finalizers;

    auto session = _InternetOpenW(
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
            [&_InternetCloseHandle, &session]()
            {
                _InternetCloseHandle(session);
                std::cout << "Closed session" << std::endl;
            }));

    auto request = _InternetOpenUrlW(
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
            [&_InternetCloseHandle, &request]()
            {
                _InternetCloseHandle(request);
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
        if (!_InternetReadFile(request, buffer, LITE_SHELL_BUFFER_SIZE, &write))
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
