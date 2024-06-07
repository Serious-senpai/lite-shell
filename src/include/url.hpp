#pragma once

#include "utils.hpp"

namespace utils
{
    /**
     * @brief Represents a URL and its components.
     */
    class URL
    {
    private:
        URL(
            const std::string &scheme,
            const std::string &hostname,
            const std::string &username,
            const std::string &password,
            const INTERNET_PORT &port,
            const std::string &path,
            const std::string &extra_info)
            : scheme(scheme),
              hostname(hostname),
              username(username),
              password(password),
              port(port),
              path(path),
              extra_info(extra_info) {}

    public:
        /** @brief The URL scheme */
        const std::string scheme;

        /** @brief The URL hostname */
        const std::string hostname;

        /** @brief The URL username */
        const std::string username;

        /** @brief The URL password */
        const std::string password;

        /** @brief The URL port */
        const INTERNET_PORT port;

        /** @brief The URL path */
        const std::string path;

        /** @brief Extra information of the URL (query strings, fragments, ...) */
        const std::string extra_info;

        /**
         * @brief Parse a string into a `URL` object
         *
         * @param url The string to parse
         * @return A `URL` object
         */
        static URL parse(const std::string &url);
    };

    URL URL::parse(const std::string &url)
    {
        URL_COMPONENTSW components;
        ZeroMemory(&components, sizeof(components));
        components.dwStructSize = sizeof(components);
        components.dwSchemeLength = (DWORD)-1;
        components.dwHostNameLength = (DWORD)-1;
        components.dwUserNameLength = (DWORD)-1;
        components.dwPasswordLength = (DWORD)-1;
        components.dwUrlPathLength = (DWORD)-1;
        components.dwExtraInfoLength = (DWORD)-1;

        const auto wurl = utf_convert(url);
        if (!InternetCrackUrlW(wurl.c_str(), wurl.size(), 0, &components))
        {
            throw std::runtime_error(last_error("wininet.h InternetCrackUrlW ERROR"));
        }

        const auto scheme = utf_convert(std::wstring(components.lpszScheme, components.dwSchemeLength));
        const auto hostname = utils::utf_convert(std::wstring(components.lpszHostName, components.dwHostNameLength));
        const auto username = utils::utf_convert(std::wstring(components.lpszUserName, components.dwUserNameLength));
        const auto password = utils::utf_convert(std::wstring(components.lpszPassword, components.dwPasswordLength));
        const auto port = components.nPort;
        const auto path = utils::utf_convert(std::wstring(components.lpszUrlPath, components.dwUrlPathLength));
        const auto extra_info = utils::utf_convert(std::wstring(components.lpszExtraInfo, components.dwExtraInfoLength));

        return URL(scheme, hostname, username, password, port, path, extra_info);
    }
}
