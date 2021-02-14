#include "PMM.hpp"
#include <panic/panic.hpp>
#include <klibc/klibc.hpp>

#define PAGE_SIZE 4096

struct FreePage {
	uint64_t npages;
	FreePage* next;
};

FreePage* top = nullptr;

void pushToQueue(uint64_t base, uint64_t size) {
	auto* freePage = (FreePage*)base;
	freePage->npages = size / PAGE_SIZE;
	freePage->next = top;
	top = freePage;
}

void PMM::init(const MemoryMap& memmap) {
	for(auto const& x : memmap)
		if(x.type == STIVALE2_MMAP_USABLE)
			pushToQueue(x.base, x.length);

	// Now we have some memory, hopefully
	if(!top)
		panic(Panic::PMM_INITIALIZE);
}

void PMM::finalizeInit(const MemoryMap& memmap) {
	for(auto const& x : memmap)
		if(x.type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
			pushToQueue(x.base, x.length);
}

uint64_t PMM::alloc() {
	// Is there any memory left?
	if(!top)
		panic(Panic::OUT_OF_MEMORY);

	// Grab the first region
	auto* ret = top;
	// Anything left from this region?
	if(ret->npages > 1) {
		// Yes!
		top = (FreePage*)((uint64_t)top + PAGE_SIZE);
		top->npages = ret->npages - 1;
		top->next = ret->next;
	} else {
		// No, use the next one
		top = ret->next;
	}

	// That would be all
	return (uint64_t)ret;
}

uint64_t PMM::calloc() {
	auto ret = alloc();
	memset((void*)ret, 0, PAGE_SIZE);
	return ret;
}

// This is trivial
void PMM::free(uint64_t addr) { pushToQueue(addr & ~0xFFF, PAGE_SIZE); }

/*void PMM::_walk() {
	printf("Free (addr, npages): ");
	for(auto* it=top; it; it=it->next)
		printf("(0x%x, 0x%x) ", it, it->npages);
	printf("\n");
}*/
