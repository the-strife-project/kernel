#include "scheduler.hpp"
#include <CPU/SMP/SMP.hpp>
#include <tasks/PIDs/PIDs.hpp>

uint64_t savedKernelState_rsp = 0;
uint64_t savedKernelState[N_CALLEE_SAVED]; // callee-saved only

[[noreturn]] void schedule() {
	// TODO: is this really necessary?
	// Is boot finished?
	/*if(savedKernelState_rsp) {
		// Nope, have to return.
		uint64_t rsp = savedKernelState_rsp;
		savedKernelState_rsp = 0;	// Make sure it isn't used again.

		asm volatile("mov %0, %%rsp\n"
					 "jmp *%1"
					 :: "r"(rsp), "r"(savedKernelState_rip));
	}*/

	// Any process ready?
	PID pid = sched.get();
	if(!pid) {
		// Nothing to do
		printf("\nThere's nothing to do.\n");
		hlt();
		while(true);
	}

	// Got it
	running[whoami()] = pid;

	getTask(pid).task->dispatch();
	// Never returns

	hardPanic(Panic::DISPATCHER_RETURNED);
}
