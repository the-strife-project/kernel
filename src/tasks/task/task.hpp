#ifndef TASK_HPP
#define TASK_HPP

#include <klibc/klibc.hpp>
#include "registers.hpp"
#include <tasks/loader/loader.hpp>
#include <tasks/constants.hpp>

class Task {
private:
	// State
	Paging paging;
	GeneralRegisters regs;
	uint64_t rip, rsp;
	uint64_t rflags = BASIC_RFLAGS;
	uint64_t heapBottom, stackTop;

	// Properties
	uint64_t prog, heap, stack;
	uint64_t maxHeapBottom, maxStackTop;
	ASLR aslr;

	void mapGeneralTask(Paging);

public:
	Task() = default;
	inline Task(const Loader::LoaderInfo& load, uint64_t entry)
		: paging(load.paging), rip(entry), rsp(load.stack),
		  heapBottom(load.heap + PAGE_SIZE), stackTop(load.stack & ~0xFFF),
		  prog(load.base), heap(load.heap), stack(load.stack),
		  maxHeapBottom(load.heap + MAX_HEAP_PAGES*PAGE_SIZE),
		  maxStackTop(load.stack - MAX_STACK_PAGES*PAGE_SIZE),
		  aslr(load.aslr)
	{ mapGeneralTask(load.paging); }

	inline Paging getPaging() { return paging; }
	void moreHeap(size_t npages);
	void moreStack();
	void freeStack();
	inline ASLR& getASLR() { return aslr; }
	void dispatchSaving();
	void dispatch();
};

#endif
