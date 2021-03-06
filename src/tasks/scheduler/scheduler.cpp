#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>

Task* generalTask;
Scheduler sched;
PID* running;	// Which PID is running each CPU

static Spinlock schedLock;

void initScheduler() {
	generalTask = (Task*)(HIGHER_HALF + PMM::calloc());
	sched = Scheduler();
	running = (PID*)VMM::Private::calloc();

	// Assign null PID
	assignPID(Scheduler::SchedulerTask());
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
