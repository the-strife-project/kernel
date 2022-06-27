#include "stivale2.h"
#include <types>

__attribute__((section(".stack"), used))
uint8_t stack[PAGE_SIZE];

stivale2_tag rsdp_tag {
	.identifier = STIVALE2_STRUCT_TAG_RSDP_ID,
	.next = (uint64_t)nullptr
};

stivale2_tag memmap_tag = {
	.identifier = STIVALE2_STRUCT_TAG_MEMMAP_ID,
	.next = (uint64_t)&rsdp_tag
};

__attribute__((section(".stivale2hdr"), used))
stivale2_header header2 = {
	.entry_point = (uint64_t)nullptr,
	.stack = (uint64_t)stack + sizeof(stack),
	.flags = 0,
	.tags = (uint64_t)&memmap_tag
};

stivale2_tag* stivale2_get_tag(stivale2_struct* bootData, uint64_t id) {
	stivale2_tag* current = (stivale2_tag*)bootData->tags;
	while(current) {
		if(current->identifier == id)
			return current;

		current = (stivale2_tag*)current->next;
	}

	return nullptr;
}
