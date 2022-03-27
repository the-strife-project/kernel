#include "task.hpp"
#include <CPU/SMP/SMP.hpp>

extern "C" void asmDispatchSaving(uint64_t rsp, uint64_t rip, GeneralRegisters*, uint64_t rflags, Paging);
extern "C" void asmDispatch(uint64_t rsp, uint64_t rip, GeneralRegisters*, uint64_t rflags, Paging);

/*extern Task* generalTask;
void Task::mapGeneralTask(Paging p) {
	p.map((uint64_t)generalTask, PMM::calloc(), PAGE_SIZE, Paging::MapFlag::NX);
}*/

uint64_t Task::moreHeap(size_t npages) {
	if(npages == 0)
		return heapBottom;

	// TODO: check this doesn't overflow
	size_t sz = npages * PAGE_SIZE;

	// Reached the limit?
	// TODO: check that this doesn't overflow
	if((heapBottom + (npages*PAGE_SIZE)) >= maxHeapBottom) {
		// TODO: kill
		printf("Heap limit reached!"); hlt();
	}

	// Map
	uint64_t flags = Paging::MapFlag::NX | Paging::MapFlag::USER;
	for(size_t i=0; i<npages; ++i)
		paging.map(heapBottom+i*PAGE_SIZE, PMM::calloc(), PAGE_SIZE, flags);

	uint64_t ret = heapBottom;
	heapBottom += sz;
	return ret;
}

// TODO void Task::moreStack() {}

void Task::freeStack() {
	size_t npages = (stack - stackTop) / PAGE_SIZE;
	++npages;	// Have to count for the initial one

	auto current = stackTop & ~0xFFF;
	while(npages--) {
		auto phys = paging.getPhys(current);
		paging.unmap(current);
		PMM::free(phys);
		current += PAGE_SIZE;
	}

	aslr.free(stack);
}

void Task::dispatchSaving() {
	asmDispatchSaving(rsp, rip, &(state.regs), state.rflags, paging);
}

void Task::dispatch() {
	asmDispatch(rsp, rip, &(state.regs), state.rflags, paging);
}

void Task::saveStateSyscall() {
	// Three things: SavedState (regs + rflags), rip, rsp
	pmemcpy(&state, paging, savedState[whoami()], sizeof(SavedState));
	rip = getState().regs.rcx; // On syscall, rcx=rip
	// The stack before syscall is as follows (go read asmhandler.asm)
	uint64_t newrsp = (uint64_t)savedState[whoami()];
	newrsp += sizeof(SavedState);
	rsp = newrsp;
}
