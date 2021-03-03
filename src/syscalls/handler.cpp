#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <mem/stacks/stacks.hpp>

extern "C" [[noreturn]] void returnToAsm(Paging);

// Just add arguments as they needed. va_list doesn't work here.
extern "C" uint64_t syscall_handler(size_t op, size_t arg1) {
	uint64_t ret = 0;
	PID pid = running[whoami()];

	switch(op) {
	case Syscalls::EXIT:
		printf("Should kill");
		hlt();
		break;
	case Syscalls::MORE_HEAP:
		getTask(pid).task->moreHeap(arg1);
		break;
	default:
		// TODO: kill
		printf("Unknown syscall");
		hlt();
		break;
	}

	return ret;
}
