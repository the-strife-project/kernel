#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <mem/stacks/stacks.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <IPC/IPC.hpp>

void onlyLoader(PID pid) {
	if(pid != Loader::LOADER_PID) {
		printf("Should kill here");
		hlt();
	}
}

// Just add arguments as they needed. va_list doesn't work here.
extern "C" uint64_t syscallHandler(size_t op, size_t arg1, size_t arg2,
								   size_t arg3) {
	uint64_t ret = 0;
	PID pid = running[whoami()];
	Scheduler::SchedulerTask& stask = getTask(pid);

	bool goBack = true;

	switch(op) {
	// --- GENERIC ---
	case std::Syscalls::EXIT:
		printf("Exit value: 0x%x\n", arg1);
		hlt();
		break;
	case std::Syscalls::MORE_HEAP:
		ret = stask.task->moreHeap(arg1);
		break;
	case std::Syscalls::MMAP:
		ret = stask.task->mmap(arg1, arg2);
		break;

	// --- LOADER ONLY ---
	case std::Syscalls::BACK_FROM_LOADER:
		onlyLoader(pid);
		Loader::imBack(arg1, arg2, arg3);
		// Never returns
		hlt();
		break;
	case std::Syscalls::MAKE_PROCESS:
		onlyLoader(pid);
		ret = Loader::makeProcess();
		break;
	case std::Syscalls::ASLR_GET:
		onlyLoader(pid);
		ret = getTask(arg1).task->getASLR().getFromID(arg2, arg3);
		break;
	case std::Syscalls::MAP_IN:
		onlyLoader(pid);
		ret = Loader::mapIn(arg1, arg2, arg3);
		break;

	// --- IPC ---
	case std::Syscalls::PUBLISH:
		ret = IPC::publish(pid, arg1);
		break;
	case std::Syscalls::RESOLVE:
		ret = IPC::resolve(arg1);
		break;
	case std::Syscalls::HALT:
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
