#include "../syscalls.hpp"
#include <rng/rng.hpp>

bool sysCSPRNG(Task* task, uint64_t virt, size_t sz) {
	while(sz) {
		uint64_t page = PAGE(virt);
		size_t count = std::min(sz, PAGE_SIZE - PAGEOFF(virt));

		uint64_t phys = task->getPaging().getPhys(page);
		if(!phys)
			return false;
		phys += PAGEOFF(virt);

		getRandom((uint8_t*)phys, count);
		sz -= count;
	}

	return true;
}
