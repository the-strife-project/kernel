#ifndef PANIC_HPP
#define PANIC_HPP

#include <asm.hpp>

struct Panic {
	enum {
		TEST,
		NO_MEMORY_MAP,
		EMPTY_MEMORY_MAP,
		PMM_INITIALIZE,
		OUT_OF_MEMORY,
		TOO_MANY_REGIONS,
		PAGE_RETRACTION,
		NO_ELF_PARSER,
		ELF_PARSER_NOT_USU,
		GET_PTE_NO_PAGE,
		ALLOCATOR_INVALID_SIZE,
		ALLOC_INVALID_SIZE,
		PURE_VIRTUAL,
		COULD_NOT_GET_ASLR
	};
};

void panic(size_t id, bool doNotHalt=false);

#endif
