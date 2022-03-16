#include "loader.hpp"
#include <tasks/constants.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <tasks/PIDs/PIDs.hpp>

// This function is the handler for the MAKE_PROCESS syscall, called by the loader
// Sets up a valid (empty) process for consecutive MAP_IN operations
PID Loader::makeProcess() {
	// First, a paging object
	Paging paging;
	paging.setData((Paging::PML4E*)PMM::calloc());
	// Kernel global entry
	paging.getData()[PAGE_ENTRIES - 1] = kpaging.getData()[PAGE_ENTRIES - 1];

	// Gotta have some ASLR
	ASLR aslr;

	// Fix a base now. MAX_ELF_SIZE (1GB) shouldn't cause a problem
	uint64_t base = aslr.get(MAX_ELF_SIZE / PAGE_SIZE, GROWS_DOWNWARD, PAGE_SIZE, DO_PANIC);
	aslr.setID(ASLR_BASE_ID, base);

	// Give it some stack. One or two pages are given, next are on demand
	uint64_t stack = aslr.get(MAX_STACK_PAGES, GROWS_DOWNWARD, STACK_ALIGNMENT, DO_PANIC);
	auto stackFlags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	paging.map((stack & ~0xFFF) - PAGE_SIZE, PMM::calloc(), PAGE_SIZE, stackFlags);
	paging.map(stack & ~0xFFF, PMM::calloc(), PAGE_SIZE, stackFlags);

	// And some heap, allocated on demand
	uint64_t heap = aslr.get(MAX_HEAP_PAGES, GROWS_UPWARD, HEAP_ALIGNMENT, DO_PANIC);

	// Create task
	LoaderInfo info(paging, aslr, base, heap, stack);
	Task* task = (Task*)PMM::calloc(); // Task is private
	*task = Task(info, 0); // Second parameter, entry point, is unknown this soon

	// TODO: Parameters?

	// Create scheduler task
	Scheduler::SchedulerTask schedTask;
	schedTask.paging = paging;
	schedTask.task = task;
	PID pid = assignPID(schedTask); // SchedTask is public

	// That's it. It wasn't so hard, was it?
	return pid;
}
