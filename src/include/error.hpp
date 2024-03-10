#pragma once

#include "format.hpp"
#include "standard.hpp"

void throw_last_error(const std::string &message)
{
    throw std::runtime_error(format("%s: %d", message.c_str(), GetLastError()));
}