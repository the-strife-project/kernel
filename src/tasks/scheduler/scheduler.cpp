#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>

Task* generalTask;
Scheduler sched;
PID* running;	// Which PID is running each CPU

void initScheduler() {
	// generalTask starts as nullptr
	// It will be set in makeProcess.cpp when building "term"
	generalTask = nullptr;

	sched = Scheduler();
	// This must be public memory, it's changed in RPC
	running = (PID*)VMM::Public::calloc();

	// Reserve null PID
	assignPID(Scheduler::SchedulerTask());
}
