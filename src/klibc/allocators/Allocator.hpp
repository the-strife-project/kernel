#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <mem/VMM/VMM.hpp>
#include <klibc/hacks.hpp>
#include <panic/panic.hpp>

// A fixed size, power of two, O(1) allocator based on a stack

template<size_t _SIZE, alloc_t _alloc, free_t _free> class Allocator {
private:
	struct Node {
		Node *prev, *next;
	} __attribute__((aligned(_SIZE)));

	const size_t entriesPerPage = PAGE_SIZE / _SIZE;
	Node* top = nullptr;

	inline void push(uint64_t addr) {
		Node* n = (Node*)addr;
		n->next = top;
		n->prev = nullptr;

		if(top)
			top->prev = n;

		top = n;
	}

	inline void pop() {
		top = top->next;
		top->prev = nullptr;
	}

	void more() {
		push(_alloc());

		// Add some nodes
		Node* current = top + 1;
		size_t n = entriesPerPage;
		while(--n)
			push((uint64_t)(current++));

		// We have... Zero allocations in this page
		kpaging.getPTE((uint64_t)top)->setUsedChunks(0);
	}

public:
	Allocator() {
		if(!(isPowerOfTwo(_SIZE) && _SIZE >= 16 && _SIZE <= 2048))
			panic(Panic::ALLOCATOR_INVALID_SIZE);
	}

	uint64_t alloc() {
		// Any chunks left?
		if(!top) more();

		uint64_t ret = (uint64_t)top;
		pop();
		kpaging.getPTE(ret & ~0xFFF)->incUsedChunks();
		return ret;
	}

	void free(uint64_t addr) {
		uint64_t page = addr & ~0xFFF;

		auto* pte = kpaging.getPTE(page);
		pte->decUsedChunks();
		push(addr);

		// Is the page free now?
		if(!pte->getUsedChunks()) {
			/*
				Yes, time to free. It's fair to assume that all entries
				in the page will be in the chunk stack. Just go through
				all the entries in this page and hop them. O(1).
			*/
			Node* current = (Node*)page;
			for(size_t i=0; i<entriesPerPage; ++i) {
				if(current->prev)
					current->prev->next = current->next;
				else
					top = current->next;

				if(current->next)
					current->next->prev = current->prev;
				++current;
			}

			_free(page);
		}
	}
};

#endif
