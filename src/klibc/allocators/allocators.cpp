#include <klibc/klibc.hpp>

#define N_ALLOCATORS 8
// uint64_t is just used here as a random type with 8 bytes (size of an Allocator object)
uint64_t privAllocators[N_ALLOCATORS] = {0};

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

void* alloc(size_t sz, bool visibility) {
	size_t alloc_idx = getAllocIdx(sz);
	if(visibility == PRIVATE) {
		void* allocator = (void*)(&privAllocators[alloc_idx]);
		switch(alloc_idx) {
		case 0: return (void*)(((PrivAllocator<16>*)allocator)->alloc());
		case 1: return (void*)(((PrivAllocator<32>*)allocator)->alloc());
		case 2: return (void*)(((PrivAllocator<64>*)allocator)->alloc());
		case 3: return (void*)(((PrivAllocator<128>*)allocator)->alloc());
		case 4: return (void*)(((PrivAllocator<256>*)allocator)->alloc());
		case 5: return (void*)(((PrivAllocator<512>*)allocator)->alloc());
		case 6: return (void*)(((PrivAllocator<1024>*)allocator)->alloc());
		case 7: return (void*)(((PrivAllocator<2048>*)allocator)->alloc());
		}
	} else {
		printf("Not implemented!");
		hlt();
	}

	printf("Something went terribly wrong!");
	hlt();
	return nullptr;
}

void free(void* ptr, size_t sz, bool visibility) {
	size_t alloc_idx = getAllocIdx(sz);
	if(visibility == PRIVATE) {
		void* allocator = (void*)(&privAllocators[alloc_idx]);
		switch(alloc_idx) {
		case 0: return ((PrivAllocator<16>*)allocator)->free((uint64_t)ptr); break;
		case 1: return ((PrivAllocator<32>*)allocator)->free((uint64_t)ptr); break;
		case 2: return ((PrivAllocator<64>*)allocator)->free((uint64_t)ptr); break;
		case 3: return ((PrivAllocator<128>*)allocator)->free((uint64_t)ptr); break;
		case 4: return ((PrivAllocator<256>*)allocator)->free((uint64_t)ptr); break;
		case 5: return ((PrivAllocator<512>*)allocator)->free((uint64_t)ptr); break;
		case 6: return ((PrivAllocator<1024>*)allocator)->free((uint64_t)ptr); break;
		case 7: return ((PrivAllocator<2048>*)allocator)->free((uint64_t)ptr); break;
		}
	} else {
		printf("Not implemented!");
		hlt();
	}

	printf("Something went terribly wrong!");
	hlt();
}
