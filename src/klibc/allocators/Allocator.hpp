#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#define ALLOCATOR_MIN_SIZE 16
#define ALLOCATOR_MAX_SIZE 2048

#define PRIVATE false
#define PUBLIC true

#include <mem/VMM/VMM.hpp>
#include <klibc/utils.hpp>
#include <panic/panic.hpp>

// A fixed size, power of two, O(1) allocator based on a stack

class Allocator {
private:
	struct Node { Node *prev, *next; };

	size_t size;
	alloc_t _alloc;
	free_t _free;

	Node* top = nullptr;

	void push(uint64_t addr);
	void pop();
	void more();

public:
	Allocator() = default;

	inline Allocator(uint64_t size, alloc_t _alloc, free_t _free)
		: size(size), _alloc(_alloc), _free(_free)
	{
		if(!(isPowerOfTwo(size) && size >= ALLOCATOR_MIN_SIZE && size <= ALLOCATOR_MAX_SIZE))
			panic(Panic::ALLOCATOR_INVALID_SIZE);
	}

	void* alloc();
	void free(void* addr);
};

#endif
