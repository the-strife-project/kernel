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
void _writes(const char* str);
void printf(const char* fmt, ...);

#endif
