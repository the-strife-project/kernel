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
		size_t npages = 0;
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
	uint64_t rpcStacks[257]; // 256 + last one is always 1

	// State
	Paging paging;
	SavedState state; // regs + rflags
	uint64_t rip, rsp;
	uint64_t heapBottom, stackTop;
	PID as; // Who am I running as?
	bool locked = false; // Whether the process is waiting for WAKE
	size_t usedPages = 0; // How many pages it's using

	// Properties
	uint64_t prog, heap, stack;
	uint64_t maxHeapBottom, maxStackTop;
	ASLR aslr;

	SharedSegment* shared = nullptr;
	bool physAllowed = false;

	void mapGeneralTask(Paging, uint64_t whereami);

public:
	Task() = default;
	Task(const Loader::LoaderInfo& load, uint64_t entry, uint64_t whereami);

	inline void setAs(PID pid) { as = pid; }
	inline PID getAs() const { return as; }

	inline void setLocked(bool v) { locked = v; }
	inline bool getLocked() const { return locked; }

	inline void incUsedPages(size_t n=1) { usedPages += n; }
	inline void decUsedPages(size_t n=1) { usedPages -= n; }
	inline size_t getUsedPages() { return usedPages; }

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
	inline void setStack(uint64_t addr) { rsp = addr; }
	void destroy();

	uint64_t mmap(size_t npages, size_t prot);
	void munmap(uint64_t base, size_t npages);
	uint64_t mapPhys(uint64_t phys, size_t npages, size_t prot);

	SharedSegment* getShared() { return shared; }
	void setShared(SharedSegment* ptr) { shared = ptr; }

	bool isPhysAllowed() const { return physAllowed; }
	void allowPhys() { physAllowed = true; }
};

extern "C" void asmRestoreKernel();

#endif
