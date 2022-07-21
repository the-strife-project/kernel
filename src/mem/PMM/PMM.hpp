#ifndef PMM_HPP
#define PMM_HPP

#include <common.hpp>
#include <mem/memmap/memmap.hpp>

namespace PhysMM {
	// Metadata for a region
	struct Frame {
		uint64_t first; // First page after the frame
		uint64_t pages; // How many pages after the frame?
		//uint64_t firstFree; // First free entry in the bitmap (faster lookup)
	} __attribute__((packed));

	extern Frame** regions;
	extern uint64_t nregions;

	void init(const MemoryMap&);
	void finish(const MemoryMap&);

	uint64_t alloc(size_t npages=1);
	uint64_t calloc(size_t npages=1);
	void free(uint64_t addr, size_t npages);

	// Using free with npages=1 would be prone to errors
	inline void freeOne(uint64_t addr) { free(addr, 1); }

	// Information
	size_t getUsablePages();
	size_t getUsedPages();
};

#endif
