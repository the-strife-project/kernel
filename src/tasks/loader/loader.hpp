#ifndef LOADER_HPP
#define LOADER_HPP

#include <mem/paging/paging.hpp>
#include "ASLR/ASLR.hpp"

namespace Loader {
	struct LoaderInfo {
		Paging paging;
		ASLR aslr;
		uint64_t base, heap, stack;

		inline LoaderInfo() {}
		inline LoaderInfo(Paging paging, const ASLR& aslr, uint64_t base, uint64_t heap, uint64_t stack)
			: paging(paging), aslr(aslr), base(base), heap(heap), stack(stack)
		{}
	};

	void bootstrapLoader();
};

#endif
