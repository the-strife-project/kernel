#include "MLFQ.hpp"
#include "../scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>

void MLFQ::add(PID pid) {
	// Time to do some feedback
	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		// Whoops
		pp.release();
		return;
	}

	Scheduler::SchedulerTask& st = *(pp.get());

	size_t prio = st.prio;
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

	st.prio = prio;
	pp.release();
	rr[prio].push(pid); // This is mutex'd
	++approxSize;
}
