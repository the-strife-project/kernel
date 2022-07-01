#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <tasks/task/task.hpp>
#include "alg/MLFQ.hpp"
#include <klibc/memory/memory.hpp>
#include <CPU/SMP/SMP.hpp>

#define NULL_PID 0

/*
	This task is a fixed pointer in all the OS.
	Points to a page that is mapped differently in each page table.
	All processes' page tables have this page mapped to a physical page
	that contains the Task object for the given process.

	This way, at any point, there's only one Task mapped in public memory.
	Note that it is NOT a global page, but the pointer is always the same.
*/
extern "C" Task* generalTask;

class Scheduler {
public:
	struct SchedulerTask {
		// Used by RPC outside of paging (no access to task)
		Paging paging;

		PID parent = 0;

		// Physical (kernel private) address of the task
		Task* task = nullptr;

		// Whether last burst ended in I/O wait (true) or used all quantum (false)
		bool ioBurst = true;
		size_t lastPrio = 0; // For regular scheduling only
	};

private:
	// Sandwich MLFQ
	static const size_t N_VSRT = 3;
	static const size_t N_REGULAR = 10;
	static const size_t N_BACKGROUND = 3;

	// All of these structures are well protected by locks
	// Very soft real time (no deadlines, just max priority)
	RoundRobin vsrt[N_VSRT];
	// Regular threads
	MLFQ regular = MLFQ(N_REGULAR);
	// Background
	RoundRobin background[N_BACKGROUND];

public:
	// TODO: try to assign the same CPU to a SRT process. This has nothing
	//   to do with this scheduler, more with the currently non-existing multicore one.
	inline void addVSRT(PID pid, size_t prio) { vsrt[prio].push(pid); }
	inline void add(PID pid) { regular.add(pid); }
	inline void addBackground(PID pid, size_t prio) { background[prio].push(pid); }

	inline PID get() {
		PID ret;
		// First, check SRT threads
		for(size_t i=0; i<N_VSRT; ++i)
			if((ret = vsrt[i].pop()))
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
extern "C" PID* running; // Which PID is running on each CPU
inline void thisCoreIsNowRunning(PID pid) { running[whoami()] = pid; }
inline PID whatIsThisCoreRunning() { return running[whoami()]; }
void initScheduler();

extern "C" uint64_t savedKernelState_rsp;
extern "C" uint64_t savedKernelState[N_CALLEE_SAVED];

[[noreturn]] void schedule();

#endif
