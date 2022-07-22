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

	uint64_t phys = PhysMM::calloc(npages);
	if(!phys)
		return 0;

	paging.map(ret, phys, npages * PAGE_SIZE, flags);
	incUsedPages(npages);
	return ret;
}

void Task::munmap(size_t base, size_t npages) {
	if(PAGEOFF(base))
		return;

	aslr.free(base);

	for(size_t i=0; i<npages; ++i) {
		uint64_t phys = paging.getPhys(base);
		paging.unmap(base);

		auto* pte = kpaging.getPTE(phys);
		size_t used = pte->getUsedChunks();
		if(used < 2) {
			// 0, not shared, so free
			// 1, shared, once, so free

			PhysMM::free(phys, 1);
		} else {
			kpaging.getPTE(phys)->decUsedChunks();
		}

		base += PAGE_SIZE;
	}

	decUsedPages(npages);
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

	// This doesn't count for usedPages

	return ret;
}
