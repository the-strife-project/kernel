#ifndef TASK_HPP
#define TASK_HPP

#include <klibc/klibc.hpp>
#include "registers.hpp"
#include <tasks/loader/loader.hpp>

/*
	Tasks will be in public dynamic memory, that is, always present,
	and not allocated temporarily, they're freed when the process dies.

	For this, and as there might be a lot of tasks, the size of this class
	in memory must be a power of two so no memory is wasted.
*/

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

public:
	Task() = default;
	inline Task(const Loader::LoaderInfo& load, uint64_t entry)
		: paging(load.paging), rip(entry), rsp(load.stack),
		  heapBottom(load.heap + PAGE_SIZE), stackTop(load.stack & ~0xFFF),
		  prog(load.base), heap(load.heap), stack(load.stack)
	{}

	void moreHeap(size_t npages=1);
	void moreStack();
	void resume();
};

#endif
