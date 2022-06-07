#include "task.hpp"
#include <syscalls>

uint64_t Task::mmap(size_t npages, size_t prot) {
	// Grab a region
	uint64_t ret = aslr.get(npages, GROWS_UPWARD, PAGE_SIZE, DO_NOT_PANIC);
	if(!ret)
		return 0;

	// Permissions
		size_t flags = 0;
		if(!(prot & std::MMAP_WRITE)) flags |= Paging::MapFlag::RO;
		if(!(prot & std::MMAP_EXEC))  flags |= Paging::MapFlag::NX;
		flags |= Paging::MapFlag::USER;

	uint64_t virt = ret;
	while(npages--) {
		// Allocate
		uint64_t page = PMM::calloc();

		// Map time
		paging.map(virt, page, PAGE_SIZE, flags);

		virt += PAGE_SIZE;
	}

	return ret;
}

uint64_t Task::mapPhys(uint64_t phys, size_t npages, size_t prot) {
	// Like above, but easier
	uint64_t ret = aslr.get(npages, GROWS_UPWARD, PAGE_SIZE, DO_NOT_PANIC);
	if(!ret)
		return 0;

	size_t flags = 0;
	if(!(prot & 0b1)) flags |= Paging::MapFlag::RO;
	if(prot & 0b10) flags |= Paging::MapFlag::PCD;
	flags |= Paging::MapFlag::NX;
	flags |= Paging::MapFlag::USER;

	uint64_t virt = ret;
	while(npages--) {
		paging.map(virt, phys, PAGE_SIZE, flags);
		virt += PAGE_SIZE;
		phys += PAGE_SIZE;
	}

	return ret;
}
