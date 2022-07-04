#include "task.hpp"

[[noreturn]] void Task::kill(size_t reason) {
	// Lock for this PID is already acquired before this was called
	//auto pp = getTask(pid);
	//Scheduler::SchedulerTask& st = *(pp.get());

	// Am I running this PID? Before IPIs
	// In that case, change to NULL_PID

	printf("Should kill now because of 0x%x\n", reason);

	hlt();
	while(true);
}
