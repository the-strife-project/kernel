#include "rng.hpp"

// This is a stub CSPRNG, it will get done better in the future
// Pretty much copy/pasted from the stdlib
// Yes, I know this is not cryptographically secure

static inline uint64_t rol64(uint64_t x, size_t k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t xoshiro256ss(uint64_t* s) {
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

static uint64_t splitmix64(uint64_t& s) {
	uint64_t result = (s += 0x9E3779B97f4A7C15);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}

void _initCSPRNG() {
	// Refresh the entropy
	// This might be suboptimal, don't know, don't care either, it's all temporal
	// I made it up on the spot, and that's a red flag in this kind of thing
	uint64_t prev = 0;
	for(size_t i=0; i<ENTROPY_QUADS; ++i) {
		uint64_t aux = splitmix64(entropy.q[i]);
		entropy.q[i] ^= prev;
		prev = aux;
	}
}

uint64_t getRandom64() {
	return xoshiro256ss(entropy.q);
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
