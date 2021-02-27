#include "loader.hpp"
#include <mem/PMM/PMM.hpp>
#include <IDT/MyIDT.hpp>

#define MAX_PROG_PAGES 1048576
#define MAX_HEAP_PAGES 16777216
#define MAX_STACK_PAGES 2048

#define PROG_ALIGNMENT PAGE_SIZE
#define HEAP_ALIGNMENT PAGE_SIZE
#define STACK_ALIGNMENT 16

void doMappings(Paging paging, uint64_t base, const PrivList<Parser::Mapping>& mappings) {
	for(auto const& x : mappings) {
		// Each mapping requieres a PageMapping object
		Paging::PageMapping map(paging, base + x.dst);
		map.setUser();
		if(!(x.flags & PARSER_FLAGS_W))
			map.setRO();
		if(!(x.flags & PARSER_FLAGS_X))
			map.setNX();

		size_t npages = x.size / PAGE_SIZE;
		size_t lastpagesz = x.size % PAGE_SIZE;
		if(lastpagesz)
			++npages;
		else
			lastpagesz = PAGE_SIZE;

		// Copy all pages but the last one
		uint64_t orig = x.orig;
		while(--npages) {
			uint64_t page = PMM::alloc();	// No need to calloc() here.
			memcpy((void*)page, (void*)orig, PAGE_SIZE);
			map.map4K(page);
			orig += PAGE_SIZE;
		}

		// Last page
		uint64_t page = PMM::calloc();
		memcpy((void*)page, (void*)orig, lastpagesz);
		map.map4K(page);
	}
}

Loader::LoaderInfo Loader::load(const PrivList<Parser::Mapping>& mappings) {
	// First create a Paging object for the process
	Paging paging;
	paging.setData((Paging::PML4E*)PMM::calloc());

	// Add the kernel global entry (last PML4E), in case the TLB gets cleared
	paging.getData()[PAGE_ENTRIES - 1] = kpaging.getData()[PAGE_ENTRIES - 1];

	ASLR aslr;

	// Do the mappings
	uint64_t base = aslr.get(MAX_PROG_PAGES, GROWS_UPWARD, PROG_ALIGNMENT);
	doMappings(paging, base, mappings);

	// Get a heap
	uint64_t heap = aslr.get(MAX_HEAP_PAGES, GROWS_UPWARD, HEAP_ALIGNMENT);
	Paging::PageMapping heapmap(paging, heap);
	heapmap.setUser();
	heapmap.setNX();
	heapmap.map4K(PMM::calloc());

	// Get a stack
	uint64_t stack = aslr.get(MAX_STACK_PAGES, GROWS_DOWNWARD, STACK_ALIGNMENT);
	Paging::PageMapping stackmap(paging, stack & ~0xFFF);
	stackmap.setUser();
	stackmap.setNX();
	stackmap.map4K(PMM::calloc());

	return LoaderInfo(paging, aslr, base, heap, stack);
}
