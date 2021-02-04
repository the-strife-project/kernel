#ifndef PMM_HPP
#define PMM_HPP

#include <common.hpp>
#include <mem/memmap/memmap.hpp>

namespace PMM {
	void init(MemoryMap&);

	uint64_t alloc();
	uint64_t calloc();
	void free(uint64_t);

	void _walk();
};

#endif
