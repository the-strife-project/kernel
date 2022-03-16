#include "ASLR.hpp"

// Used by makeProcess.cpp to set ID 0
void ASLR::setID(size_t id, uint64_t page) {
	ids.push_back(std::pair<size_t, uint64_t>(id, page));
}

/*
	Handler for the ASLR_GET syscall, used by the loader
	This syscall handles region allocations in the memory space
	Works with IDs. 0 is program base. Next are for shared libraries
	Two modes of operation:
	- id is set. Returns base.
	- id is not set. Creates a region "npages" big, then returns base.
*/
uint64_t ASLR::getFromID(size_t id, size_t npages) {
	for(auto const& x : ids)
		if(x.f == id)
			return x.s;

	// It's not there. Let's make a new one
	uint64_t ret = get(npages, GROWS_UPWARD, PAGE_SIZE, DO_NOT_PANIC);
	setID(id, ret);
	return ret;
}
