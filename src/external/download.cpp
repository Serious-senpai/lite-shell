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

    std::cout << "Scheme: " << components.nScheme << std::endl;
    std::cout << "Hostname: " << hostname << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Password: " << password << std::endl;
    std::cout << "Port: " << components.nPort << std::endl;
    std::cout << "Path: " << urlpath << std::endl;
    std::cout << "Extra info: " << extrainfo << std::endl;

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

    /*
    auto connection = _InternetConnectW(
        session,                              // hInternet
        utils::utf_convert(hostname).c_str(), // lpszServerName
        components.nPort,                     // nServerPort
        utils::utf_convert(username).c_str(), // lpszUserName
        utils::utf_convert(password).c_str(), // lpszPassword
        INTERNET_SERVICE_HTTP,                // dwService
        0,                                    // dwFlags
        0                                     // dwContext
    );
    if (connection == NULL)
    {
        throw std::runtime_error(utils::last_error("wininet.h InternetConnectW ERROR"));
    }
    */

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

    /*
    if (!_HttpSendRequestW(
            request, // hRequest
            NULL,    // lpszHeaders
            0,       // dwHeadersLength
            NULL,    // lpOptional
            0        // dwOptionalLength
            ))
    {
        throw std::runtime_error(utils::last_error("wininet.h HttpSendRequestW ERROR"));
    }
    */

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

        std::cout << "Downloaded bytes: " << total << "         \r" << std::flush;
    } while (write > 0);

    _InternetCloseHandle(request);
    // _InternetCloseHandle(connection);
    _InternetCloseHandle(session);

    return 0;
}
