#include "../syscalls.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <kkill>

void wait(PID parent, PID child) {
	// Parent is acquired already

	// Did child already die?
	auto pp = getTask(parent);
	auto& stask = *(pp.get());

	for(auto& x : stask.children) {
		if(x.pid == child) {
			if(x.exited)
				return; // Nothing to do

			x.waiting = true;
			stask.task->saveStateSyscall();
			pp.release();
			schedule();
		}
	}

	return;
}
