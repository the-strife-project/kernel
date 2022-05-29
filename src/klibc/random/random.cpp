#include "random.hpp"

uint64_t getRandom64() {
	// Placeholder
	uint64_t ret;
	asm volatile("rdrand %%rax" : "=a"(ret));
	return ret;
}
