#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <tasks/task/task.hpp>
#include "alg/MLFQ.hpp"
#include <klibc/memory/memory.hpp>
#include <CPU/SMP/SMP.hpp>
#include <loader>
#include <kkill>

#define NULL_PID 0
#define UID_SYSTEM 1

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

		// Who is the user responsible?
		size_t uid = 0;

		// Who called EXEC on me
		PID parent = 0;

		// If I called EXEC, the return value of the loader
		size_t lastLoaderError = std::Loader::Error::NONE;

		// Children
		struct Child {
			PID pid = NULL_PID;
			size_t kr = std::kkill::OK; // Kill reason
			size_t ret = ~0ull; // Exit value
			bool waiting = false; // Is parent waiting?
			bool exited = false; // Did this child exit?
		};
		List<Child> children;

		// Physical (kernel private) address of the task
		Task* task = nullptr;

		// Whether last burst ended in I/O wait (true) or used all quantum (false)
		bool ioBurst = true;
		size_t prio = 0;
		size_t prioType = 1; // Default is regular

		// Some methods
		void exit(size_t code);
		void kill(PID me, size_t reason);
		void _commonDie(PID me, size_t reason, size_t code);
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
	inline void addRegular(PID pid) { regular.add(pid); }
	inline void addBackground(PID pid, size_t prio) { background[prio].push(pid); }

	void add(PID pid);

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
extern "C" PID* origRunning; // Which PID is running on each CPU
extern "C" PID* runningAs; // RPC PID

inline void setOrigRunning(PID pid) {
	auto who = whoami();
	origRunning[who] = runningAs[who] = pid;
}
inline PID getOrigRunning() { return origRunning[whoami()]; }
inline void setRunningAs(PID pid) { runningAs[whoami()] = pid; }
inline PID getRunningAs() { return runningAs[whoami()]; }

void initScheduler();

extern "C" uint64_t savedKernelState_rsp;
extern "C" uint64_t savedKernelState[N_CALLEE_SAVED];

[[noreturn]] void schedule();

#endif
