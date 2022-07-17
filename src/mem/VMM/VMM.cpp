#include "VMM.hpp"
#include <klibc/spinlock.hpp>

static Spinlock lock = Spinlock();
inline void atomicMap(uint64_t virt, uint64_t phys) {
	lock.acquire();
	kpaging.map(virt, phys, PAGE_SIZE, Paging::MapFlag::NX | Paging::MapFlag::GLOBAL);
	lock.release();
}

uint64_t PublicMM::alloc(size_t npages) {
	uint64_t phys = PhysMM::alloc(npages);
	uint64_t ret = HIGHER_HALF + phys;
	atomicMap(ret, phys);
	return ret;
}

uint64_t PublicMM::calloc(size_t npages) {
	uint64_t phys = PhysMM::calloc(npages);
	uint64_t ret = HIGHER_HALF + phys;
	atomicMap(ret, phys);
	return ret;
}

void PublicMM::free(uint64_t x, size_t npages) {
	// Unmap and free
	lock.acquire();
	uint64_t page = PAGE(x);
	PhysMM::free(page - HIGHER_HALF, npages);

	while(npages--) {
		kpaging.unmap(page);
		invlpg(page);
		page += PAGE_SIZE;
	}
	lock.release();
}
