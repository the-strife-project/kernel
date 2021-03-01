#include "scheduler.hpp"

Task* generalTask;
Scheduler sched;
PubMLvector<PID> running;	// Which PID is running each CPU
PubMLvector<Scheduler::SchedulerTask> tasks;

void initScheduler(size_t CPUs) {
	generalTask = (Task*)(HIGHER_HALF + PMM::calloc());
	sched = Scheduler();
	running = PubMLvector<PID>();
	tasks = PubMLvector<Scheduler::SchedulerTask>();

	while(CPUs--)
		running[running.next()] = NULL_PID;
}

void Scheduler::addFront(PID pid) {
	// Lock
	rr.push_front(pid);
}

void Scheduler::add(PID pid) {
	// Lock
	rr.push(pid);
}

PID Scheduler::get() {
	// Lock
	return rr.pop();
}

PID assignPID(const Scheduler::SchedulerTask& task) {
	// Lock
	PID pid = tasks.next();
	tasks[pid] = task;
	return pid;
}
