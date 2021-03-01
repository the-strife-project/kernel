#include "task.hpp"

extern "C" void actuallyResumeTask(uint64_t rsp, uint64_t rip, GeneralRegisters*, uint64_t rflags);

extern Task* generalTask;
void Task::mapGeneralTask(Paging p) {
	p.map((uint64_t)generalTask, PMM::calloc(), PAGE_SIZE, Paging::MapFlag::NX);
}

void Task::moreHeap(size_t npages) {
	if(npages == 0)
		return;

	size_t sz = npages * PAGE_SIZE;

	// Reached the limit?
	if((heapBottom + sz) >= maxHeapBottom) {
		// TODO: kill
		printf("Heap limit reached!"); hlt();
	}

	// Map
	uint64_t flags = Paging::MapFlag::NX | Paging::MapFlag::USER;
	paging.map(heapBottom, PMM::calloc(), sz, flags);
	heapBottom += npages*PAGE_SIZE;
}

void Task::resume() {
	actuallyResumeTask(rsp, rip, &regs, rflags);
}
