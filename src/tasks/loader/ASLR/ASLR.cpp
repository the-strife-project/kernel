#include "ASLR.hpp"

#define MIN_ADDRESS 0x0000000000100000
#define MAX_ADDRESS 0x00007FFFFFFFFFFF
#define MAX_ITERATIONS 100

inline uint64_t rdrand() {
	uint64_t ret;
	asm volatile("rdrand %%rax" : "=a"(ret));
	return ret;
}

static uint64_t getCandidate() {
	return rdrand();	// Stub
}

inline bool overlaps(uint64_t begin, uint64_t end, uint64_t xbegin, uint64_t xend) {
	bool ret = xbegin > end;
	ret = ret || (xend < begin);
	return !ret;
}

// Aligns without crossing a page boundary
uint64_t align(uint64_t x, uint64_t alignment) {
	uint64_t diff = x % alignment;

	uint64_t below = x - diff;
	uint64_t above = x + (alignment - diff);
	if((x & ~0xFFF) == (below & ~0xFFF))
		return below;
	return above;
}

uint64_t ASLR::get(size_t max_pages, bool direction, uint64_t alignment, bool doNotPanic) {
	if(max_pages && direction && alignment) {}

	for(uint64_t it=0; it<MAX_ITERATIONS; ++it) {
		uint64_t candidate = getCandidate();
		candidate %= MAX_ADDRESS - MIN_ADDRESS;
		candidate += MIN_ADDRESS;

		uint64_t begin = candidate & ~0xFFF;
		uint64_t end = (candidate + max_pages*PAGE_SIZE) & ~0xFFF;

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
