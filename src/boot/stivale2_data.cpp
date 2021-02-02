#include "stivale2.h"

__attribute__((section(".stack"), used))
uint8_t stack[4096];

__attribute__((section(".stivale2hdr"), used))
stivale2_header header2 = {
	.entry_point = (uint64_t)nullptr,
	.stack = (uint64_t)stack + sizeof(stack),
	.flags = 0,
	.tags = (uint64_t)nullptr
};
