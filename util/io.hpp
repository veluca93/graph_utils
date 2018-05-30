#ifndef IO_HPP
#define IO_HPP
#include <stdio.h>
#include <iostream>
#include "strtk.hpp"

void AddToBuffer(const std::string& s);

template <typename T>
void write(const T& param) {
    AddToBuffer(strtk::type_to_string(param));
}

template<typename First, typename... Args>
void write(const First& first, const Args&... args) {
    write(first);
    write(args...);
}
size_t nextInt(bool comments = false);

#endif
