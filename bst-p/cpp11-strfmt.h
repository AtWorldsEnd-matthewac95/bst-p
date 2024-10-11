#pragma once
#include <string>
#include <memory>
#include <exception>

template <class... Args> std::string string_format(std::string const & format, Args ... args) {
    int rawSize = 1 + std::snprintf(nullptr, 0, format.c_str(), args ...);

    if (rawSize <= 0) {
        throw std::exception{"Bad string format passed to cpp11-strfmt::string_format."};
    }

    size_t size = static_cast<size_t>(rawSize);
    char * buffer = new char[size];
    std::snprintf(buffer, size, format.c_str(), args ...);

    std::string formatted{buffer, buffer + size - 1};
    delete [] buffer;
    return formatted;
}
