#include "Allocator.hpp"

#define ENTRIES_PER_PAGE (PAGE_SIZE / size)

void Allocator::push(uint64_t addr) {
	Node* n = (Node*)addr;
	n->next = top;
	n->prev = nullptr;

	if(top)
		top->prev = n;

	top = n;
}

void Allocator::pop() {
	top = top->next;
	if(top)
		top->prev = nullptr;
}

void Allocator::more() {
	push(_alloc(1));

	// Add some nodes
	uint64_t current = (uint64_t)top + size;
	size_t n = ENTRIES_PER_PAGE;
	while(--n) {
		push(current);
		current += size;
	}

	// We have... Zero allocations in this page
	kpaging.getPTE((uint64_t)top)->setUsedChunks(0);
}



void* Allocator::alloc() {
	// Any chunks left?
	if(!top) more();

	uint64_t ret = (uint64_t)top;
	pop();
	kpaging.getPTE(PAGE(ret))->incUsedChunks();
	return (void*)ret;
}

void Allocator::free(void* addr) {
	uint64_t page = PAGE((uint64_t)addr);

	auto* pte = kpaging.getPTE(page);
	pte->decUsedChunks();
	push((uint64_t)addr);

	// Is the page free now?
	if(!pte->getUsedChunks()) {
		/*
			Yes, time to free. It's fair to assume that all entries
			in the page will be in the chunk stack. Just go through
			all the entries in this page and hop them. O(1).
		*/
		uint64_t current = page;
		uint64_t n = ENTRIES_PER_PAGE;
		while(n--) {
			Node* node = (Node*)current;
			if(node->prev)
				node->prev->next = node->next;
			else
				top = node->next;

			if(node->next)
				node->next->prev = node->prev;

			current += size;
		}

		_free(page, 1);
	}
}
