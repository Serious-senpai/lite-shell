#pragma once

#include "utils.hpp"

namespace utils
{
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
            const std::string &extra_info);

    public:
        const std::string scheme;
        const std::string hostname;
        const std::string username;
        const std::string password;
        const INTERNET_PORT port;
        const std::string path;
        const std::string extra_info;

        static URL parse(const std::string &url);
    };
}
