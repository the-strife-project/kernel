#ifndef PANIC_HPP
#define PANIC_HPP

#include <klibc/klibc.hpp>
#include <asm.hpp>

struct Panic {
	enum {
		TEST,
		NO_MEMORY_MAP,
		EMPTY_MEMORY_MAP,
		PMM_INITIALIZE,
		OUT_OF_MEMORY
	};
};

void panic(size_t id, bool doNotHalt=false);

#endif
