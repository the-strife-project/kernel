#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>

Task* generalTask;
Scheduler sched;
PID* origRunning;
PID* runningAs;

void initScheduler() {
	// generalTask starts as nullptr
	// It will be set in makeProcess.cpp when building "term"
	generalTask = nullptr;

	sched = Scheduler();
	origRunning = (PID*)VMM::Private::calloc();
	// This must be public memory, it's changed in RPC
	runningAs = (PID*)VMM::Public::calloc();

	// Reserve null PID
	assignPID(Scheduler::SchedulerTask());
}
