#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <mem/stacks/stacks.hpp>

extern "C" [[noreturn]] void returnToAsm(Paging);
void exportProcedure(Scheduler::SchedulerTask&, uint64_t);

// Just add arguments as they needed. va_list doesn't work here.
extern "C" uint64_t syscall_handler(size_t op, size_t arg1) {
	uint64_t ret = 0;
	PID pid = running[whoami()];
	Scheduler::SchedulerTask& stask = getTask(pid);

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
	default:
		// TODO: kill
		printf("Unknown syscall");
		hlt();
		break;
	}

	return ret;
}
