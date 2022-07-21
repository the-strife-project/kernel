#include "loader.hpp"
#include <tasks/constants.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <tasks/PIDs/PIDs.hpp>

// This function is the handler for the MAKE_PROCESS syscall, called by the loader
// Sets up a valid (empty) process for consecutive MAP_IN operations
PID Loader::makeProcess() {
	// First, a paging object
	Paging paging;
	paging.setData((Paging::PML4E*)PhysMM::calloc());
	// Kernel global entry
	paging.getData()[PAGE_ENTRIES - 1] = kpaging.getData()[PAGE_ENTRIES - 1];

	// Gotta have some ASLR
	ASLR aslr;

	// generalTask
	if(!generalTask) {
		// Time to fix it
		uint64_t val = aslr.get(1, GROWS_DOWNWARD, PAGE_SIZE, DO_PANIC);
		generalTask = (Task*)val;
	} else {
		// Already set, just mark it in ASLR
		uint64_t val = (uint64_t)generalTask;
		aslr.set(val, 1);
	}

	// Fix a base now. MAX_ELF_SIZE (1GB) shouldn't cause a problem
	uint64_t base = aslr.get(MAX_ELF_SIZE / PAGE_SIZE, GROWS_DOWNWARD, PAGE_SIZE, DO_PANIC);
	aslr.setID(ASLR_BASE_ID, base);

	// Give it some stack. One or two pages are given, next are on demand
	uint64_t stack = aslr.get(MAX_STACK_PAGES, GROWS_DOWNWARD, STACK_ALIGNMENT, DO_PANIC);
	auto stackFlags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	paging.map(PAGE(stack) - PAGE_SIZE, PhysMM::calloc(), PAGE_SIZE, stackFlags);

	// And some heap, allocated on demand
	uint64_t heap = aslr.get(MAX_HEAP_PAGES, GROWS_UPWARD, HEAP_ALIGNMENT, DO_PANIC);

	// Create task
	LoaderInfo info(paging, aslr, base, heap, stack);
	Task* task = (Task*)PhysMM::calloc(); // Task is private
	uint64_t entrypoint = 0; // Unknown this soon
	*task = Task(info, entrypoint, (uint64_t)task);
	task->incUsedPages(); // Stack

	// Create scheduler task
	Scheduler::SchedulerTask schedTask;
	schedTask.task = task;
	schedTask.paging = paging;
	PID pid = assignPID(schedTask); // SchedTask is public
	task->setAs(pid);

	// That's it. It wasn't so hard, was it?
	return pid;
}
