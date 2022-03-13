#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <mem/stacks/stacks.hpp>
#include <tasks/PIDs/PIDs.hpp>

//extern "C" [[noreturn]] void returnToAsm(Paging); // <- fix
void exportProcedure(Scheduler::SchedulerTask&, uint64_t);

// Just add arguments as they needed. va_list doesn't work here.
extern "C" uint64_t syscallHandler(size_t op, size_t arg1, size_t arg2) {
	uint64_t ret = 0;
	PID pid = running[whoami()];
	Scheduler::SchedulerTask& stask = getTask(pid);

	bool goBack = true;

	switch(op) {
	case std::Syscalls::EXIT:
		printf("Should kill");
		hlt();
		break;
	case std::Syscalls::MORE_HEAP:
		ret = stask.task->moreHeap(arg1);
		break;
	case std::Syscalls::BACK_FROM_LOADER:
		Loader::imBack(arg1, arg2);
		// Never returns
		hlt();
		break;
	case std::Syscalls::MAKE_PROCESS:
		printf("Make process");
		hlt();
		break;
	/*case std::Syscalls::EXPORT:
		exportProcedure(stask, arg1);
		break;
	case std::Syscalls::HALT:
		stask.task->freeStack();
		goBack = false;
		break;*/
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
