#include "stivale2.h"

__attribute__((section(".stivale2hdr"), used))
stivale2_header header2 = {
	.entry_point = (uint64_t)nullptr,
	.stack = (uint64_t)nullptr,
	.flags = 0,
	.tags = (uint64_t)nullptr
};
