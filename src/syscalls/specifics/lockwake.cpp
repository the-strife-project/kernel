#include "../syscalls.hpp"
#include <tasks/PIDs/PIDs.hpp>

void lockCurrent() {
	std::PID pid = getOrigRunning();
	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		pp.release();
		return;
	}

	auto& stask = *(pp.get());
	// Save state and set as locked
	stask.task->saveStateSyscall();
	stask.task->setLocked(true);
	stask.ioBurst = true;

	pp.release();
}

bool wake(PID pid) {
	// I'm aware this is insecure

	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		pp.release();
		return false;
	}

	auto& stask = *(pp.get());

	// Was it locked?
	if(!stask.task->getLocked()) {
		pp.release();
		return false;
	}

	stask.task->setLocked(false);
	pp.release();

	sched.add(pid);
	return true;
}
