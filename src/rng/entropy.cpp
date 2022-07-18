#include "rng.hpp"

static bool gotrdrand = false;
Entropy entropy;

static uint64_t rdrand() {
	uint64_t ret;
	asm volatile("rdrand %%rax" : "=a"(ret));
	return ret;
}

void _initCSPRNG();

void initRandom() {
	size_t ecx;
	asm volatile("cpuid"
				 : "=c" (ecx)
				 : "a" (1)
				 : "cc", "memory");
	gotrdrand = ecx & CPUID_RDRAND;

	// Fill entropy (TODO, do better)
	if(gotrdrand) {
		for(size_t i=0; i<ENTROPY_QUADS; ++i)
			entropy.q[i] = rdrand();
	} else {
		for(size_t i=0; i<ENTROPY_QUADS; ++i)
			entropy.q[i] = 42;
	}

	_initCSPRNG();
}
