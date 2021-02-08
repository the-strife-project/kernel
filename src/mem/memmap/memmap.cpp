#include "memmap.hpp"
#include <panic/panic.hpp>

MemoryMap::MemoryMap(stivale2_struct* bootData) {
	auto* tag = (stivale2_struct_tag_memmap*)stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_MEMMAP_ID);

	if(!tag)
		panic(Panic::NO_MEMORY_MAP);

	entries = tag->entries;
	data = tag->memmap;
}

void MemoryMap::move(stivale2_mmap_entry* x) {
	// TODO: memcpy
	for(size_t i=0; i<entries; ++i)
		x[i] = data[i];
	data = x;
}
