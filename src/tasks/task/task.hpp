#ifndef TASK_HPP
#define TASK_HPP

#include <klibc/klibc.hpp>
#include "registers.hpp"
#include <tasks/loader/loader.hpp>
#include <tasks/constants.hpp>

void pmemcpy(void* dst, Paging remote, void* orig, size_t n);

class Task {
private:
	// State
	Paging paging;
	SavedState state; // regs + rflags
	uint64_t rip, rsp;
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
		  heapBottom(load.heap), stackTop(load.stack & ~0xFFF),
		  prog(load.base), heap(load.heap), stack(load.stack),
		  maxHeapBottom(load.heap + MAX_HEAP_PAGES*PAGE_SIZE),
		  maxStackTop(load.stack - MAX_STACK_PAGES*PAGE_SIZE),
		  aslr(load.aslr)
	{ mapGeneralTask(load.paging); }

	inline Paging getPaging() { return paging; }
	uint64_t moreHeap(size_t npages);
	void moreStack();
	void freeStack();
	inline ASLR& getASLR() { return aslr; }
	void dispatchSaving();
	void dispatch();
	inline SavedState& getState() { return state; }
	void saveStateSyscall();

	inline void jump(uint64_t addr) { rip = addr; }

	uint64_t mmap(size_t npages, size_t prot);
};

extern "C" void asmRestoreKernel();

#endif
