#include "PIDs.hpp"

#include <klibc/klibc.hpp>

// Last GB of virtual memory for PIDs
static Scheduler::SchedulerTask** tasks = (Scheduler::SchedulerTask**)(0 - (1 << 30));

size_t givenPIDs = 0;
static Spinlock lock;

PID assignPID(const Scheduler::SchedulerTask& task) {
	lock.acquire();
	if(givenPIDs % (PAGE_SIZE / sizeof(void*)) == 0) {
		// Need a new page
		uint64_t virt = (uint64_t)&tasks[givenPIDs];
		kpaging.map(virt, PMM::calloc(), PAGE_SIZE, Paging::MapFlag::NX | Paging::MapFlag::GLOBAL);
	}

	tasks[givenPIDs] = (Scheduler::SchedulerTask*)alloc(sizeof(Scheduler::SchedulerTask), PUBLIC);
	*(tasks[givenPIDs]) = task;
	PID ret = givenPIDs++;
	lock.release();
	return ret;
}

Scheduler::SchedulerTask& getTask(PID pid) {
	lock.acquire();
	auto* ret = tasks[pid];
	lock.release();
	return *ret;
}
