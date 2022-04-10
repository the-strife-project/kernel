#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <tasks/task/task.hpp>
#include "alg/MLFQ.hpp"
#include <klibc/memory/memory.hpp>

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
//extern Task* generalTask;

class Scheduler {
public:
	struct SchedulerTask {
		PID parent = 0;

		// Physical (private) address of the task
		Task* task = nullptr;

		// Exported procedures (page)
		uint64_t* procs = nullptr;
		uint64_t nprocs = 0;

		// Stacks for concurrency
		uint64_t* stacks = nullptr;

		// Whether last burst ended in I/O wait (true) or used all quantum (false)
		bool ioBurst = true;
		size_t lastPrio = 0; // For regular scheduling only
	};

private:
	// Sandwich MLFQ
	static const size_t N_SRT = 3;
	static const size_t N_REGULAR = 10;
	static const size_t N_BACKGROUND = 3;

	// All of these structures are well protected by locks
	// Soft real time threads
	RoundRobin srt[N_SRT];
	// Regular threads
	MLFQ regular = MLFQ(N_REGULAR);
	// Background
	RoundRobin background[N_BACKGROUND];

public:
	// TODO: try to assign the same CPU to a SRT process. This has nothing
	//   to do with this scheduler, more with the currently non-existing multicore one.
	inline void addSRT(PID pid, size_t prio) { srt[prio].push(pid); }
	inline void add(PID pid) { regular.add(pid); }
	inline void addBackground(PID pid, size_t prio) { background[prio].push(pid); }

	inline PID get() {
		PID ret;
		// First, check SRT threads
		for(size_t i=0; i<N_SRT; ++i)
			if((ret = srt[i].pop()))
				return ret;
		// Then, regular ones
		if((ret = regular.pop()))
			return ret;
		// If there's nothing to do, run background tasks
		for(size_t i=0; i<N_BACKGROUND; ++i)
			if((ret = background[i].pop()))
				return ret;
		return 0;
	}
};

extern Scheduler sched;
extern PID* running; // Which PID is running on each CPU
void initScheduler();

extern "C" uint64_t savedKernelState_rsp;
extern "C" uint64_t savedKernelState[N_CALLEE_SAVED];

[[noreturn]] void schedule();

Scheduler::SchedulerTask& getMyCurrent();

#endif
