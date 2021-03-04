#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>

void exportProcedure(Scheduler::SchedulerTask& stask, uint64_t addr) {
	if(!stask.procs)
		stask.procs = (uint64_t*)VMM::Public::calloc();

	// Check that the the address is mapped and executable
	uint64_t flags = stask.task->getPaging().getFlags(addr);
	if((flags == (uint64_t)~0) || flags & Paging::MapFlag::NX) {
		printf("Should kill."); hlt();
	}

	// All set
	stask.procs[stask.nprocs++] = addr;

	// Grab some stacks if they haven't been set yet
	if(!stask.stacks) {
		stask.stacks = (uint64_t*)VMM::Public::calloc();
		for(size_t i=0; i<ncores; ++i) {
			auto stack = stask.task->getASLR().get(1, GROWS_DOWNWARD, 16);
			uint64_t flags = Paging::MapFlag::NX | Paging::MapFlag::USER;
			stask.task->getPaging().map(stack & ~0xFFF, PMM::alloc(), PAGE_SIZE, flags);
			stask.stacks[i] = stack;
		}
	}
}
