#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>

extern PID vfsPID;

void Scheduler::SchedulerTask::exit(size_t code) {
	// If exit() was called, it surely was in this core, no doubt
	PID me = whatIsThisCoreRunning();

	// Special case: VFS and code 99
	if(me == vfsPID && code == 99)
		panic(Panic::NO_AHCI);

	// Critical process?
	if(!parent)
		panic(Panic::CRITICAL_PROCESS_EXITED);

	// Let parent know their child exited
	auto pp = getTask(parent);
	pp.acquire();
	// Alive? 🤨
	if(!pp.isNull()) {
		// Good, set return value
		SchedulerTask* stask = pp.get();
		for(auto& x : stask->children) {
			if(x.pid == me) {
				x.kr = std::kkill::OK; // Not killed
				x.ret = code;
				if(x.waiting) {
					// Parent is waiting, wake them up (TODO)
				}
			}
		}
	}
	pp.release();

	// Finally, destroy the task
	this->task->destroy();
	// TODO: remember to set null pointer in PID table
}
