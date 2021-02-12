#ifndef KLIBC_MEMORY_HPP
#define KLIBC_MEMORY_HPP

#include <common.hpp>

/*
	TODO: Check if fast-string operation is enabled
	https://patchwork.kernel.org/project/linux-acpi/patch/e6c5440954c28e9738b8d4919a9db2d10c9a1757.1313586473.git.luto@mit.edu/
*/

// Neat ERMSB
inline void memcpy(void* dst, void* orig, size_t n) {
	asm volatile("rep movsb" :: "D"(dst), "S"(orig), "c"(n) : "memory", "cc");
}

inline void memset(void* ptr, uint8_t value, size_t n) {
	asm volatile("rep stosb" :: "D"(ptr), "a"(value), "c"(n) : "memory", "cc");
}

#endif
