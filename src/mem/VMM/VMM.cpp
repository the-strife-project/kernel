#include "VMM.hpp"

uint64_t VMM::Private::alloc() { return PMM::alloc(); }
void VMM::Private::free(uint64_t x) { return PMM::free(x); }

// Don't need a public VMM right now

/*struct Node {
	uint64_t next;
	uint64_t size;
};

uint64_t VMM_first = 0;
uint64_t VMM_last = 0;

#define PUBLIC_BEGIN 0xFFFFFFFF00000000

uint32_t VMM::Public::alloc() {
	return 0;
}*/
