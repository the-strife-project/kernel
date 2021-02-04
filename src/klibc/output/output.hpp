#ifndef KLIBC_OUTPUT_HPP
#define KLIBC_OUTPUT_HPP

#include <stdarg.h>
#include <common.hpp>

/*
		WARNING

These functions will only be used at boot, and in case of
extreme failure. Most of the time:
(i)  The kernel won't need to output anything
(ii) If it does have to, it will call the term driver.
*/

void _writec(char c);

inline void _writes(const char* str) {
	while(*str)
		_writec(*str++);
}

void printf(const char* fmt, ...);
void setColor(uint8_t color);
void resetColor();

#endif
