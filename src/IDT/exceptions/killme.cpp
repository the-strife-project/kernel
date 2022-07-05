#include "exceptions.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <tasks/scheduler/scheduler.hpp>

// Many exceptions result in the death of the process that caused them
[[noreturn]] void exceptionKill(size_t reason) {
	PID me = getRunningAs();

	auto pp = getTask(me);
	pp.acquire();
	if(pp.isNull()) {
		// No biggie
		setOrigRunning(NULL_PID);
		pp.release();
		schedule();
	}

	pp.get()->kill(me, reason);
	pp.release();

	schedule();
}
