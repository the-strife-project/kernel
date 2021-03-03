#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <tasks/task/task.hpp>
#include "alg/RR.hpp"

#define NULL_PID 0

/*
	This task is a fixed pointer in all the OS.
	Points to a page that is mapped differently in each page table.
	All processes' page tables have this page mapped to a physical page
	that contains the Task object for the given process.

	This way, at any point, there's only one Task mapped in public memory.
	It is NOT a global page, but the pointer is always the same.

	TODO: this might not be needed, check again when the kernel is done.
*/
extern Task* generalTask;

class Scheduler {
public:
	struct SchedulerTask {
		PID parent = 0;

		/*
			This is an exact copy of generalTask->paging.
			Except that, well, generalTask requires being on the page table
			of the process.
		*/
		Paging paging;

		// Physical (private) address of the task
		Task* task;

		// Exported procedures (page)
		uint64_t* procs = nullptr;
		uint64_t nprocs = 0;

		// Stacks for concurrency
		uint64_t* stacks = nullptr;
	};

private:
	// TODO: add priorities (vector of RRs).
	RoundRobin rr;

public:
	void add(PID);
	void addFront(PID);
	PID get();
};

extern Scheduler sched;
extern PubMLvector<PID> running;
void initScheduler(size_t CPUs);
PID assignPID(const Scheduler::SchedulerTask&);
Scheduler::SchedulerTask& getTask(PID);

#endif
