#include "rng.hpp"

// This is a stub CSPRNG, it will get done better in the future

// Yes, I know this is not cryptographically secure

static inline uint64_t rol64(uint64_t x, size_t k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t xorshiro256ss(uint64_t* s) {
	uint64_t ret = rol64(s[1] * 5, 7) * 9;
	uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;
	s[3] ^= rol64(s[3], 45);

	return ret;
}

uint64_t getRandom64() {
	return xorshiro256ss(entropy.q);
}

void getRandom(uint8_t* data, size_t bytes) {
	uint64_t buffer;
	size_t used = 0;
	for(size_t i=0; i<bytes; ++i) {
		if(!used) {
			buffer = getRandom64();
			used = 8;
		}

		*(data++) = buffer & 0xFF;
		buffer >>= 8;
		--used;
	}
}
