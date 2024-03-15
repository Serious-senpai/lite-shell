#pragma once

#include "standard.hpp"

class Client;

class Namespace
{
private:
    std::map<std::string, std::string> str_variables;
    std::map<std::string, long long> int_variables;

public:
    const Client *client;

    Namespace(const Client *client) : client(client) {}
};