#include "exceptions.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <tasks/scheduler/scheduler.hpp>

// Many exceptions result in the death of the process that caused them
[[noreturn]] void exceptionKill(size_t reason) {
	auto pp = getTask(whatIsThisCoreRunning());
	pp.acquire();
	if(pp.isNull()) {
		// No biggie
		thisCoreIsNowRunning(NULL_PID);
		pp.release();
		schedule();
	}

	Task* task = pp.get()->task;
	task->kill(reason);
}
