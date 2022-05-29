#include "task.hpp"
#include <syscalls>

uint64_t Task::mmap(size_t npages, size_t prot) {
	// Grab a region
	uint64_t ret = aslr.get(npages, GROWS_UPWARD, PAGE_SIZE, DO_NOT_PANIC);

	if(!ret)
		return 0;

	uint64_t virt = ret;
	for(size_t i=0; i<npages; ++i) {
		// Allocate
		uint64_t page = PMM::calloc();

		// Set permissions
		size_t flags = 0;
		if(!(prot & std::MMAP_WRITE)) flags |= Paging::MapFlag::RO;
		if(!(prot & std::MMAP_EXEC))  flags |= Paging::MapFlag::NX;
		flags |= Paging::MapFlag::USER;

		// Map time
		paging.map(virt, page, PAGE_SIZE, flags);

		virt += PAGE_SIZE;
	}

	return ret;
}
