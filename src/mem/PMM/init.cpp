#include "PMM.hpp"
#include <panic/panic.hpp>
#include <klibc/memory/memory.hpp>
#include <klibc/klibc.hpp>

PhysMM::Frame** PhysMM::regions = nullptr;
uint64_t PhysMM::nregions = 0;

static size_t usablePages = 0;

static void initialize(uint64_t base, uint64_t length) {
	uint64_t npages = length / PAGE_SIZE;
	usablePages += npages;

	uint64_t bmapSize = (npages + 8 - 1) / 8;
	// How many pages for metadata?
	uint64_t needed = sizeof(PhysMM::Frame) + bmapSize;
	uint64_t metadata = NPAGES(needed);

	// That's it, format the region
	PhysMM::Frame* frame = (PhysMM::Frame*)base;
	frame->first = base + metadata * PAGE_SIZE;
	frame->pages = npages - metadata;
	//frame->firstFree = 0;

	// Let's clean the bitmap
	void* firstBitmap = (void*)(base + sizeof(PhysMM::Frame));
	memset(firstBitmap, 0, bmapSize);
}

// Stivale2 guarantees that both "usable" and "bootloader reclaimable" entries
// are PAGE_SIZE aligned for both base and length
// (https://github.com/stivale/stivale/blob/master/STIVALE2.md).
void PhysMM::init(const MemoryMap& memmap) {
	Frame* first = nullptr;

	for(auto const& x : memmap) {
		switch(x.type) {
		case STIVALE2_MMAP_USABLE:
			if(x.length < 2 * PAGE_SIZE)
				break;
			initialize(x.base, x.length);
			if(!first)
				first = (Frame*)x.base;
			++nregions;
		}
	}

	// Now we have some memory, hopefully
	if(!first)
		panic(Panic::PMM_INITIALIZE);

	// Let's get an array of regions
	// This helps in freeing
	size_t needed = NPAGES(nregions * sizeof(Frame*));

	// Set the first region for now
	regions = &first; // In the stack. Cool, huh?
	regions = (Frame**)PhysMM::calloc(needed); // Get actually needed

	// And set them
	Frame** ptr = regions;
	for(auto const& x : memmap) {
		switch(x.type) {
		case STIVALE2_MMAP_USABLE:
			if(x.length < 2 * PAGE_SIZE)
				break;
			*(ptr++) = (Frame*)x.base;
		}
	}
}

size_t PhysMM::getUsablePages() { return usablePages; }

/*void PhysMM::finish(const MemoryMap& memmap) {
	size_t ctr = 0;
	for(auto const& x : memmap) {
		switch(x.type) {
		case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
			if(x.length < 2 * PAGE_SIZE)
				break;
			initialize(x.base, x.length);
			++ctr;
		}
	}

	// Recreate "regions"
	Frame** old = regions;
	size_t needed = NPAGES((nregions + ctr) * sizeof(Frame*));
	regions = (Frame**)PhysMM::calloc(needed);
	Frame** ptr = regions;

	// Put new values
	for(auto const& x : memmap) {
		switch(x.type) {
		case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
			if(x.length < 2 * PAGE_SIZE)
				break;
			*(ptr++) = (Frame*)x.base;
		}
	}

	// Put old values
	for(size_t i=0; i<nregions; ++i)
		*(ptr++) = *(old++);
}*/
