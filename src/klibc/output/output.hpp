#ifndef KLIBC_OUTPUT_HPP
#define KLIBC_OUTPUT_HPP

#include <stdarg.h>
#include <common.hpp>

#define PHYS_VIDEO_BASE 0xB8000
#define VIDEO_BASE 0xFFFFFFFF800B8000
#define FB_COLS 80
#define FB_ROWS 25
#define FB_BYTES_PER_ROW (FB_COLS * 2)
#define DEFAULT_COLOR 0x0A

/*
		WARNING

These functions will only be used at boot, and in case of
extreme failure. Most of the time, the kernel won't need to
output anything. In case it does (probably for debugging),
it will keep the cursor synchronized with the term driver.
*/

void _writec(char c);

inline void _writes(const char* str) {
	while(*str)
		_writec(*str++);
}

void printf(const char* fmt, ...);
void setColor(uint8_t color);
void resetColor();

void nowSyncWithTerm(size_t* kcursor);

#endif
