#include "task.hpp"
#include <CPU/SMP/SMP.hpp>
#include <panic/bruh.hpp>
#include <tasks/PIDs/PIDs.hpp>

extern "C" void asmDispatchSaving(uint64_t rsp, uint64_t rip, GeneralRegisters*, uint64_t rflags, Paging);
extern "C" void asmDispatch(uint64_t rsp, uint64_t rip, GeneralRegisters*, uint64_t rflags, Paging);

extern "C" Task* generalTask;
void Task::mapGeneralTask(Paging p, uint64_t whereami) {
	// Task is always on its own page, so that makes this easy
	if(generalTask) {
		// Set, so map it
		p.map((uint64_t)generalTask, whereami, PAGE_SIZE, Paging::MapFlag::NX);
	}
}

Task::Task(const Loader::LoaderInfo& load, uint64_t entry, uint64_t whereami)
	: paging(load.paging), rip(entry), rsp(load.stack),
	  heapBottom(load.heap), stackTop(PAGE(load.stack)),
	  prog(load.base), heap(load.heap), stack(load.stack),
	  maxHeapBottom(load.heap + MAX_HEAP_PAGES*PAGE_SIZE),
	  maxStackTop(load.stack - MAX_STACK_PAGES*PAGE_SIZE),
	  aslr(load.aslr)
{
	mapGeneralTask(load.paging, whereami);

	// No RPC stacks allocated
	for(size_t i=0; i<257; ++i)
		rpcStacks[i] = 1;
}

uint64_t Task::moreHeap(size_t npages) {
	if(npages == 0)
		return heapBottom;

	// TODO: check this doesn't overflow
	size_t sz = npages * PAGE_SIZE;

	// Reached the limit?
	// TODO: check that this doesn't overflow
	if((heapBottom + (npages*PAGE_SIZE)) >= maxHeapBottom)
		return 0;

	// Map
	uint64_t flags = Paging::MapFlag::NX | Paging::MapFlag::USER;
	uint64_t phys = PhysMM::calloc(npages);
	if(!phys)
		return 0;
	paging.map(heapBottom, phys, npages * PAGE_SIZE, flags);

	uint64_t ret = heapBottom;
	heapBottom += sz;
	incUsedPages(npages);
	return ret;
}

// TODO void Task::moreStack() {}

void Task::freeStack() {
	size_t npages = PAGE(stackTop);
	npages -= PAGE(stack);
	npages /= PAGE_SIZE;
	++npages;

	auto current = PAGE(stackTop);
	while(npages--) {
		auto phys = paging.getPhys(current);
		paging.unmap(current);
		// Might be some issues with offsets: stackTop might be in a non-allocated
		//   page, which is perfectly valid. That's why the check below
		if(phys)
			PhysMM::freeOne(phys);
		current += PAGE_SIZE;
		decUsedPages();
	}

	aslr.free(stack);
}

void Task::dispatchSaving() {
	asmDispatchSaving(rsp, rip, &(state.regs), state.rflags, paging);
}

void Task::dispatch() {
	// If this is about to dispatch, then origRunning (myself) is set
	PID myself = origRunning[whoami()];

	if(myself == as) {
		// Regular dispatching
		asmDispatch(rsp, rip, &(state.regs), state.rflags, paging);
	} else {
		// Similar, but different page table
		auto pp = getTask(as);
		pp.acquire();
		if(pp.isNull()) {
			printf("TODO");
			while(true);
		}

		Paging p = pp.get()->paging;
		pp.release();

		asmDispatch(rsp, rip, &(state.regs), state.rflags, p);
	}
}

void Task::saveStateSyscall() {
	// Who am I running as?
	as = getRunningAs();

	// Get the SavedState
	auto pp = getTask(as);
	// Must be acquired at this point (handler does it at the beginning)
	Paging p = pp.get()->paging;
	// And there it goes
	pmemcpy(&state, p, savedState[whoami()], sizeof(SavedState));

	// Finally, rip, flags, and rsp
	rip = getState().regs.rcx; // On syscall, rcx=rip
	rpcFlags = getState().regs.r11; // And r11=flags
	// The stack before syscall is as follows (go read asmhandler.asm)
	uint64_t newrsp = (uint64_t)savedState[whoami()];
	newrsp += sizeof(SavedState);
	rsp = newrsp;
}
