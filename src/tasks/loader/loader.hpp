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
	void imBack(PID pid, size_t err, uint64_t entry);

	extern PID last_pid;
	extern size_t last_err;
	extern size_t last_entry;

	// Place where ELFs are loaded in the loader
	const uint64_t ELF_BASE = 128ull * (1ull << 30);
	// Max ELF size
	const uint64_t MAX_ELF_SIZE = 1ull << 30;

	extern PID LOADER_PID;
	void freeELF();
	bool mapELF(uint64_t elf, size_t sz, bool doNotPanic=false);
	bool movePage(uint64_t phys, size_t idx);

	PID makeProcess();
	bool mapIn(PID pid, uint64_t local, uint64_t remote);
};

#endif
