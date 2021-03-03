#include "VMM.hpp"
#include <klibc/spinlock.hpp>

uint64_t VMM::Private::alloc() { return PMM::alloc(); }
uint64_t VMM::Private::calloc() { return PMM::calloc(); }
void VMM::Private::free(uint64_t x) { return PMM::free(x); }

static Spinlock lock = Spinlock();
inline void atomicMap(uint64_t virt, uint64_t phys) {
	lock.acquire();
	kpaging.map(virt, phys, PAGE_SIZE, Paging::MapFlag::NX | Paging::MapFlag::GLOBAL);
	lock.release();
}

uint64_t VMM::Public::alloc() {
	uint64_t phys = PMM::alloc();
	uint64_t ret = HIGHER_HALF + phys;
	atomicMap(ret, phys);
	return ret;
}

uint64_t VMM::Public::calloc() {
	uint64_t phys = PMM::calloc();
	uint64_t ret = HIGHER_HALF + phys;
	atomicMap(ret, phys);
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
