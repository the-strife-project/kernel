#ifndef KLIBC_MEMORY_HPP
#define KLIBC_MEMORY_HPP

#include <common.hpp>

extern "C" void* memcpy(void* dst, void* orig, size_t n);
extern "C" void* memmove(void* dst, void* orig, size_t n);
extern "C" void* memset(void* ptr, char value, size_t n);

#endif
