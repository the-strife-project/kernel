#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <mem/stacks/stacks.hpp>
#include <tasks/PIDs/PIDs.hpp>

extern "C" [[noreturn]] void returnToAsm(Paging);
void exportProcedure(Scheduler::SchedulerTask&, uint64_t);

// Just add arguments as they needed. va_list doesn't work here.
extern "C" uint64_t syscallHandler(size_t op, size_t arg1) {
	uint64_t ret = 0;
	PID pid = running[whoami()];
	Scheduler::SchedulerTask& stask = getTask(pid);

	bool goBack = true;

	switch(op) {
	case Syscalls::EXIT:
		printf("Should kill");
		hlt();
		break;
	case Syscalls::MORE_HEAP:
		stask.task->moreHeap(arg1);
		break;
	case Syscalls::EXPORT:
		exportProcedure(stask, arg1);
		break;
	case Syscalls::HALT:
		stask.task->freeStack();
		goBack = false;
		break;
	default:
		// TODO: kill
		printf("Unknown syscall");
		hlt();
		break;
	}

	if(!goBack)
		schedule();

	return ret;
}
