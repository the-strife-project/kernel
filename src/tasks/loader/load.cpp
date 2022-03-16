#include "loader.hpp"
#include "../PIDs/PIDs.hpp"

void Loader::freeELF() {
	// Go through the ELF pages in the loader process, unmapping and freeing
	uint64_t page = ELF_BASE;
	Paging paging = getTask(LOADER_PID).task->getPaging();

	while(true) {
		uint64_t phys = paging.getPhys(page);
		if(!phys) break;

		// Unmap
		paging.unmap(page);
		// Free, from PMM (private pages)
		PMM::free(phys);

		page += PAGE_SIZE;
	}
}

static void howManyPages(size_t size, size_t& npages, size_t& lastpagesz) {
	npages = size / PAGE_SIZE;
	lastpagesz = size % PAGE_SIZE;
	if(lastpagesz)
		++npages;
	else
		lastpagesz = PAGE_SIZE;
}

// A more specific version of bootstrap.cpp:_mapInLoader
static void copyELF(Paging paging, uint64_t begin, size_t sz) {
	size_t npages, lastpagesz;
	howManyPages(sz, npages, lastpagesz);

	Paging::PageMapping map(paging, Loader::ELF_BASE);
	map.setUser();
	map.setRO();
	map.setNX();

	// Copy all pages but the last one
	while(--npages) {
		uint64_t page = PMM::alloc();	// No need to calloc() here.
		memcpy((void*)page, (void*)begin, PAGE_SIZE);
		map.map4K(page);
		begin += PAGE_SIZE;
	}

	// Last page
	uint64_t page = PMM::calloc();
	memcpy((void*)page, (void*)begin, lastpagesz);
	map.map4K(page);
}

bool Loader::mapELF(uint64_t elf, size_t sz, bool doNotPanic) {
	if(sz > MAX_ELF_SIZE) {
		if(doNotPanic)
			return false;
		else
			panic(Panic::BOOTSTRAP_ELF_TOO_BIG);
	}

	Paging paging = getTask(Loader::LOADER_PID).task->getPaging();
	copyELF(paging, elf, sz);
	return true;
}
