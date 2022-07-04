#ifndef RNG_HPP
#define RNG_HPP

#include <types>

#define CPUID_RDRAND (1 << 30)
#define ENTROPY_BITS 128
#define ENTROPY_BYTES (ENTROPY_BITS / 8)
#define ENTROPY_QUADS (ENTROPY_BYTES / sizeof(uint64_t))

union Entropy {
	uint8_t  b[ENTROPY_BYTES];
	uint64_t q[ENTROPY_QUADS];
};
extern Entropy entropy;

void initRandom();
void getRandom(uint8_t* data, size_t bytes);

inline uint64_t getRandom64() {
	volatile uint64_t ret;
	getRandom((uint8_t*)&ret, sizeof(uint64_t));
	return ret;
}

#endif
