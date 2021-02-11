#ifndef VMM_HPP
#define VMM_HPP

#include <mem/paging/paging.hpp>
#include <mem/PMM/PMM.hpp>

/*
	Private pages are referred to with their physical address (lower half).
	Public pages are marked as global so they're available to all contexts.
*/

typedef uint64_t (*alloc_t)();
typedef void (*free_t)(uint64_t);

namespace VMM {
	namespace Private {
		uint64_t alloc();
		void free(uint64_t);
	}

	namespace Public {
		uint64_t alloc();
		void free(uint64_t);
	}
};

#endif
