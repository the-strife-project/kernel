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
	for(size_t i=0; i<npages; ++i)
		paging.map(heapBottom+i*PAGE_SIZE, PhysMM::calloc(), PAGE_SIZE, flags);

	uint64_t ret = heapBottom;
	heapBottom += sz;
	return ret;
}

// TODO void Task::moreStack() {}

void Task::freeStack() {
	size_t npages = stackTop & ~0xFFF;
	npages -= stack & ~0xFFF;
	npages /= PAGE_SIZE;
	++npages;

	auto current = stackTop & ~0xFFF;
	while(npages--) {
		auto phys = paging.getPhys(current);
		paging.unmap(current);
		// Might be some issues with offsets: stackTop might be in a non-allocated
		//   page, which is perfectly valid. That's why the check below
		if(phys)
			PhysMM::freeOne(phys);
		current += PAGE_SIZE;
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
	// Three things: SavedState (regs + rflags), rip, rsp
	pmemcpy(&state, paging, savedState[whoami()], sizeof(SavedState));
	rip = getState().regs.rcx; // On syscall, rcx=rip
	rpcFlags = getState().regs.r11; // And r11=flags
	as = runningAs[whoami()]; // There's the last runningAs
	// The stack before syscall is as follows (go read asmhandler.asm)
	uint64_t newrsp = (uint64_t)savedState[whoami()];
	newrsp += sizeof(SavedState);
	rsp = newrsp;
}
