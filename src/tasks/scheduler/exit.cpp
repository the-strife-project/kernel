#include "scheduler.hpp"
#include <tasks/PIDs/PIDs.hpp>

extern PID vfsPID;

void Scheduler::SchedulerTask::exit(size_t code) {
	// If exit() was called, it surely was in this core, no doubt
	PID me = getRunningAs();

	// Special case: VFS and code 99
	if(me == vfsPID && code == 99)
		panic(Panic::NO_AHCI);

	// Critical process?
	if(!parent) {
		panic(Panic::CRITICAL_PROCESS_DIED, true);
		printf("\nIt was PID 0x%x, exit value: 0x%x", me, code);
		hlt();
	}

	_commonDie(me, std::kkill::OK, code);
}

void Scheduler::SchedulerTask::kill(PID me, size_t reason) {
	// Here it's not possible to get the PID so it's necessary as parameter
	if(!parent) {
		panic(Panic::CRITICAL_PROCESS_DIED, true);
		printf("\nIt was PID 0x%x, reason: 0x%x", me, reason);
		hlt();
	}

	_commonDie(me, reason, ~0ull);
}

void Scheduler::SchedulerTask::_commonDie(PID me, size_t reason, size_t code) {
	// My protected pointer
	auto mypp = getTask(me);
	// Must be acquired at this point!
	mypp.assertAcquired();

	// Let cores know I have died (TODO, IPIs)

	// Let parent know their child exited
	auto ppp = getTask(parent);
	ppp.acquire();
	bool released = false;
	// Alive? 🤨
	if(!ppp.isNull()) {
		// Good, set return value
		SchedulerTask* stask = ppp.get();
		for(auto& x : stask->children) {
			if(x.pid == me) {
				x.kr = reason;
				x.ret = code;
				x.exited = true;
				if(x.waiting) {
					// Parent is waiting, wake them up
					ppp.release();
					sched.add(parent);
					released = true;
				}
			}
		}
	}

	if(!released)
		ppp.release();

	// Destroy the PCB
	this->task->destroy();
	PhysMM::freeOne((uint64_t)(this->task));
	this->task = nullptr;

	// Free myself. Weird, huh?
	free(this, sizeof(Scheduler::SchedulerTask), PUBLIC);

	// Set nullptr in PID table for whoever comes next
	mypp.setNull();
	// And we're done

	// TODO: if runningAs != origRunning, kill callers
}
