#include <klibc/klibc.hpp>

#define N_ALLOCATORS 8
Allocator pubAllocators[N_ALLOCATORS];
Allocator privAllocators[N_ALLOCATORS];

size_t getAllocIdx(size_t sz) {
	if(sz > 2048)
		panic(Panic::ALLOC_INVALID_SIZE);
	else if(sz < 16)
		sz = 16;

	size_t alloc_idx = log2(sz) - 4;
	if(!isPowerOfTwo(sz))
		++alloc_idx;

	return alloc_idx;
}

void initAllocators() {
	for(size_t i=0; i<N_ALLOCATORS; ++i) {
		Allocator pub(16 << i, PublicMM::alloc, PublicMM::free);
		Allocator priv(16 << i, PhysMM::alloc, PhysMM::free);
		pubAllocators[i] = pub;
		privAllocators[i] = priv;
	}
}

inline Allocator* getAllocator(size_t sz, bool visibility) {
	size_t idx = getAllocIdx(sz);
	if(visibility == PUBLIC)
		return &pubAllocators[idx];
	else
		return &privAllocators[idx];
}

void* alloc(size_t sz, bool visibility) {
	return getAllocator(sz, visibility)->alloc();
}

void free(void* ptr, size_t sz, bool visibility) {
	return getAllocator(sz, visibility)->free(ptr);
}
