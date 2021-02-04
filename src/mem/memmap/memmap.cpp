#include "memmap.hpp"
#include <klibc/klibc.hpp>
#include <asm.hpp>

MemoryMap::MemoryMap(stivale2_struct* bootData) {
	auto* tag = (stivale2_struct_tag_memmap*)stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_MEMMAP_ID);

	if(!tag) {
		// TODO: Actually panic
		printf("PANIC: Limine didn't pass the memory map");
		hlt(); while(true);
	}

	entries = tag->entries;
	data = tag->memmap;
}
