#include "PMM.hpp"
#include <panic/panic.hpp>
#include <klibc/klibc.hpp>

#define PAGE_SIZE 4096

struct FreePage {
	uint64_t npages;
	FreePage* next;
};

FreePage* first = nullptr;
FreePage* last = nullptr;

void pushToQueue(uint64_t base, uint64_t size) {
	auto* freePage = (FreePage*)base;

	if(!first)
		first = freePage;
	freePage->npages = size / PAGE_SIZE;
	freePage->next = nullptr;
	if(last)
		last->next = freePage;
	last = freePage;
}

void PMM::init(MemoryMap& memmap) {
	for(auto const& x : memmap)
		if(x.type == STIVALE2_MMAP_USABLE || x.type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
			pushToQueue(x.base, x.length);

	// Now we have some memory, hopefully
	if(!first)
		panic(Panic::PMM_INITIALIZE);
}

uint64_t PMM::alloc() {
	// Is there any memory left?
	if(!first)
		panic(Panic::OUT_OF_MEMORY);

	// Grab the first region
	auto* ret = first;
	// Anything left from this region?
	if(ret->npages > 1) {
		// Yes!
		first = (FreePage*)((uint64_t)first + PAGE_SIZE);
		first->npages = ret->npages - 1;
		first->next = ret->next;
	} else {
		// No, use the next one
		first = ret->next;
	}

	// That would be all
	return (uint64_t)ret;
}

uint64_t PMM::calloc() {
	auto ret = alloc();
	// TODO: memset
	for(size_t i=0; i<(PAGE_SIZE/sizeof(uint64_t)); ++i)
		*(uint64_t*)(ret+i) = 0;
	return ret;
}

// This is trivial
void PMM::free(uint64_t addr) { pushToQueue(addr & ~0xFFF, PAGE_SIZE); }

void PMM::_walk() {
	printf("Free (addr, npages): ");
	for(auto* it=first; it; it=it->next)
		printf("(0x%x, 0x%x) ", it, it->npages);
	printf("\n");
}
