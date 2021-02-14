#include "loader.hpp"
#include <mem/PMM/PMM.hpp>
#include <IDT/MyIDT.hpp>

#define BASE_LOAD 0x100000
#define BASE_STACK 0xFFFFFF8000000000

void doMappings(Paging paging, const PrivList<Parser::Mapping>& mappings) {
	for(auto const& x : mappings) {
		// Each mapping requieres a PageMapping object
		Paging::PageMapping map(paging, BASE_LOAD + x.dst);
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

	// Do the mappings
	doMappings(paging, mappings);

	// Get a stack
	Paging::PageMapping stackmap(paging, BASE_STACK - PAGE_SIZE);
	stackmap.setUser();
	stackmap.setNX();
	stackmap.map4K(PMM::calloc());

	return LoaderInfo(paging, BASE_LOAD, BASE_STACK);
}
