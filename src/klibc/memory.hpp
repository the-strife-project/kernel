#ifndef KLIBC_MEMORY_HPP
#define KLIBC_MEMORY_HPP

#include <common.hpp>

// Neat ERMSB
inline void memcpy(void* dst, void* orig, size_t n) {
	asm volatile("rep movsb" :: "D"(dst), "S"(orig), "c"(n) : "memory");
}

//void memset(void* ptr, uint8_t value, size_t n);

#endif
