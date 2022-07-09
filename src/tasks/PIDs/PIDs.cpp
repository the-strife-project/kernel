#include "PIDs.hpp"

#include <klibc/klibc.hpp>
#include <panic/bruh.hpp>

// This is static. It cannot be accessed from the outside. Must use getTask().
static Scheduler::SchedulerTask** tasks = (Scheduler::SchedulerTask**)GLOBAL_PIDS_REGION;

static size_t givenPIDs = 0;

//void preparePIDs() {} // TODO allocate all pages

PID assignPID(const Scheduler::SchedulerTask& task) {
	// TODO: full?
	if(givenPIDs % (PAGE_SIZE / sizeof(void*)) == 0) {
		// Need a new page
		uint64_t virt = (uint64_t)&tasks[givenPIDs];
		kpaging.map(virt, PhysMM::calloc(), PAGE_SIZE, Paging::MapFlag::NX | Paging::MapFlag::GLOBAL);
	}

	tasks[givenPIDs] = (Scheduler::SchedulerTask*)alloc(sizeof(Scheduler::SchedulerTask), PUBLIC);
	*(tasks[givenPIDs]) = task;
	return givenPIDs++;
}

ProtPtr<Scheduler::SchedulerTask> getTask(PID pid) {
	return ProtPtr<Scheduler::SchedulerTask>(&tasks[pid]);
}
