#include "VMM.hpp"
#include <klibc/spinlock.hpp>

uint64_t VMM::Private::alloc() { return PMM::alloc(); }
void VMM::Private::free(uint64_t x) { return PMM::free(x); }

static Spinlock lock = Spinlock();

uint64_t VMM::Public::alloc() {
	// Alloc and map :^)
	lock.acquire();
	uint64_t phys = PMM::alloc();
	uint64_t ret = HIGHER_HALF + phys;
	Paging::PageMapping mapping(kpaging, ret);
	mapping.setNX();
	mapping.setGlobal();
	mapping.map4K(phys);
	lock.release();
	return ret;
}

void VMM::Public::free(uint64_t x) {
	// Unmap and free
	lock.acquire();
	uint64_t page = x & ~0xFFF;
	Paging::PageMapping mapping(kpaging, page);
	mapping.setNotPresent();
	mapping.map4K((uint64_t)nullptr);
	invlpg(page);
	PMM::free(page - HIGHER_HALF);
	lock.release();
}
