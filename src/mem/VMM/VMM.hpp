#ifndef VMM_HPP
#define VMM_HPP

#include <mem/paging/paging.hpp>
#include <mem/PMM/PMM.hpp>

/*
	Private pages are referred to with their physical address (lower half).
	Public pages are marked as global so they're available to all contexts.
*/

typedef uint64_t (*alloc_t)(size_t npages);
typedef void (*free_t)(uint64_t ptr, size_t npages);

namespace PublicMM {
	uint64_t alloc(size_t npages=1);
	uint64_t calloc(size_t npages=1);
	void free(uint64_t ptr, size_t npages);
	inline void freeOne(uint64_t ptr) { free(ptr, 1); }
};

#endif
