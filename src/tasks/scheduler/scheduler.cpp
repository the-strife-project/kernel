#include "scheduler.hpp"

Task* generalTask;
Scheduler sched;
PubMLvector<PID> running;	// Which PID is running each CPU
static PubMLvector<Scheduler::SchedulerTask> tasks;

static Spinlock schedLock, PIDLock;

void initScheduler(size_t CPUs) {
	generalTask = (Task*)(HIGHER_HALF + PMM::calloc());
	sched = Scheduler();
	running = PubMLvector<PID>();
	tasks = PubMLvector<Scheduler::SchedulerTask>();

	while(CPUs--)
		running[running.next()] = NULL_PID;
}

void Scheduler::addFront(PID pid) {
	schedLock.acquire();
	rr.push_front(pid);
	schedLock.release();
}

void Scheduler::add(PID pid) {
	schedLock.acquire();
	rr.push(pid);
	schedLock.release();
}

PID Scheduler::get() {
	schedLock.acquire();
	auto ret = rr.pop();
	schedLock.release();
	return ret;
}

PID assignPID(const Scheduler::SchedulerTask& task) {
	PIDLock.acquire();
	PID pid = tasks.next();
	tasks[pid] = task;
	PIDLock.release();
	return pid;
}
