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
	origRunning = (PID*)PhysMM::calloc();
	// This must be public memory, it's changed in RPC
	runningAs = (PID*)PublicMM::calloc();

	// Reserve null PID
	assignPID(Scheduler::SchedulerTask());
}

void Scheduler::add(PID pid) {
	auto pp = getTask(pid);
	pp.acquire();
	size_t prioType = pp.get()->prioType;
	size_t prio = pp.get()->prio;
	pp.release();

	switch(prioType) {
	case 0:
		addVSRT(pid, prio);
		break;
	case 1:
		addRegular(pid);
		break;
	case 2:
		addBackground(pid, prio);
		break;
	}
}
