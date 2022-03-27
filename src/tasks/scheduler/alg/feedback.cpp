#include "MLFQ.hpp"
#include "../scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>

void MLFQ::add(PID pid) {
	// Time to do some feedback
	Scheduler::SchedulerTask& st = getTask(pid);

	size_t prio = st.lastPrio;
	if(st.ioBurst) {
		// Task is waiting for I/O. Promotion.
		if(prio > 0)
			--prio;
	} else {
		// Task consumed all quantum. Demotion.
		if(prio < nprio-1)
			++nprio;
	}

	// TODO: What about aging?

	st.lastPrio = prio;
	rr[prio].push(pid); // This is mutex'd
	++approxSize;
}
