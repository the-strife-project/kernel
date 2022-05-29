#ifndef TASK_HPP
#define TASK_HPP

#include <klibc/klibc.hpp>
#include "registers.hpp"
#include <tasks/loader/loader.hpp>
#include <tasks/constants.hpp>

bool pmemcpy(void* dst, Paging remote, void* orig, size_t n);

class Task {
public:
	struct SharedSegment {
		std::SMID smid = 0;
		uint64_t kptr = 0;
		uint64_t tptr = 0;
		std::PID allowed = 0;
	} __attribute__((packed));
	static const size_t NUM_SHARED_SEGMENTS = PAGE_SIZE / sizeof(SharedSegment);

private:
	// Used by RPC (asm), so it's important to keep this first
	uint64_t rpcEntry = 0;
	uint64_t rpcFlags = BASIC_RFLAGS; // RFLAGS when entering RPC handler

	/*
		Stacks are needed for different CPUs that can RPC simultaneously.
		Option A: one stack per CPU. Direct access, no checks, thread-safe.
			This has an issue: if process gets interrupted and it's later resumed
			by another CPU, the stack is lost.
		Option B: stack pool. When an RPC happens, handler checks if any is free,
			in which case takes it. Otherwise, allocates a new one.
		I'm pretty sure B is the way to go.
		An item in rpcStacks might be:
			0: this stack in the pool exists and it's in use
			1: from here on, there are no free stacks
			any other: available stack
	*/
	uint64_t rpcStacks[257] = {1}; // 256 + last one is always 1

	// State
	Paging paging;
	SavedState state; // regs + rflags
	uint64_t rip, rsp;
	uint64_t heapBottom, stackTop;

	// Properties
	uint64_t prog, heap, stack;
	uint64_t maxHeapBottom, maxStackTop;
	ASLR aslr;

	SharedSegment* shared = nullptr;

	void mapGeneralTask(Paging, uint64_t whereami);

public:
	Task() = default;
	inline Task(const Loader::LoaderInfo& load, uint64_t entry, uint64_t whereami)
		: paging(load.paging), rip(entry), rsp(load.stack),
		  heapBottom(load.heap), stackTop(load.stack & ~0xFFF),
		  prog(load.base), heap(load.heap), stack(load.stack),
		  maxHeapBottom(load.heap + MAX_HEAP_PAGES*PAGE_SIZE),
		  maxStackTop(load.stack - MAX_STACK_PAGES*PAGE_SIZE),
		  aslr(load.aslr)
	{ mapGeneralTask(load.paging, whereami); }

	inline uint64_t* getRPCStacks() { return rpcStacks; }
	inline Paging getPaging() { return paging; }
	uint64_t moreHeap(size_t npages);
	void moreStack();
	void freeStack();
	inline ASLR& getASLR() { return aslr; }
	void dispatchSaving();
	void dispatch();
	inline SavedState& getState() { return state; }
	void saveStateSyscall();
	inline void setRPCentry(uint64_t x) { rpcEntry = x; }

	inline void jump(uint64_t addr) { rip = addr; }
	[[noreturn]] void kill(size_t reason);

	uint64_t mmap(size_t npages, size_t prot);

	SharedSegment* getShared() { return shared; }
	void setShared(SharedSegment* ptr) { shared = ptr; }
};

extern "C" void asmRestoreKernel();

#endif
