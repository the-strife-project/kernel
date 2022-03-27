#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>

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
