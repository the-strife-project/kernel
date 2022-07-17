#include "ASLR.hpp"
#include <rng/rng.hpp>

#define MIN_ADDRESS 0x0000000000100000
#define MAX_ADDRESS 0x00007FFFFFFFFFFF
#define MAX_ITERATIONS 100

static inline bool overlaps(uint64_t begin, uint64_t end, uint64_t xbegin, uint64_t xend) {
	bool ret = xbegin > end;
	ret = ret || (xend < begin);
	return !ret;
}

static inline bool contains(uint64_t begin, uint64_t end, uint64_t x) {
	return overlaps(begin, end, x, x);
}

// Aligns without crossing a page boundary
uint64_t align(uint64_t x, uint64_t alignment) {
	uint64_t diff = x % alignment;

	uint64_t below = x - diff;
	uint64_t above = x + (alignment - diff);
	if(PAGE(x) == PAGE(below))
		return below;
	return above;
}

uint64_t ASLR::get(size_t max_pages, bool direction, uint64_t alignment, bool doNotPanic) {
	// TODO: Make this safer

	for(uint64_t it=0; it<MAX_ITERATIONS; ++it) {
		uint64_t candidate = getRandom64();
		candidate %= MAX_ADDRESS - MIN_ADDRESS;
		candidate += MIN_ADDRESS;

		uint64_t begin = PAGE(candidate);
		uint64_t end = PAGE(candidate + max_pages*PAGE_SIZE);

		for(auto const& x : list) {
			if(overlaps(begin, end, x.begin, x.end))
				goto next;
		}

		list.push_back(Node(begin, end));
		return align((direction == GROWS_UPWARD) ? candidate : candidate + max_pages*PAGE_SIZE, alignment);

	next:
		continue;
	}

	if(!doNotPanic)
		panic(Panic::COULD_NOT_GET_ASLR);
	return 0;
}

void ASLR::set(uint64_t addr, size_t max_pages) {
	list.push_back(Node(addr, addr + max_pages*PAGE_SIZE));
}

void ASLR::free(uint64_t addr) {
	for(auto& x : list) {
		if(contains(x.begin, x.end, addr)) {
			// TODO: It isn't worth thinking right now
			x.begin = x.end = 0;
		}
	}
}
