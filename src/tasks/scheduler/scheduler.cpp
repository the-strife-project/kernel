#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>

//Task* generalTask;
Scheduler sched;
PID* running;	// Which PID is running each CPU

void initScheduler() {
	//generalTask = (Task*)(HIGHER_HALF + PMM::calloc());
	sched = Scheduler();
	running = (PID*)VMM::Private::calloc();

	// Reserve null PID
	assignPID(Scheduler::SchedulerTask());
}

Scheduler::SchedulerTask& getMyCurrent() {
	return getTask(running[whoami()]);
}
