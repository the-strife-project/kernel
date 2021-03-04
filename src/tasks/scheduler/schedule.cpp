#include "scheduler.hpp"

uint64_t savedKernelState_rsp = 0;
uint64_t savedKernelState_rip = 0;

[[noreturn]] void schedule() {
	// Is boot finished?
	if(savedKernelState_rsp) {
		// Nope, have to return.
		uint64_t rsp = savedKernelState_rsp;
		savedKernelState_rsp = 0;	// Make sure it isn't used again.

		asm volatile("mov %0, %%rsp\n"
					 "jmp *%1"
					 :: "r"(rsp), "r"(savedKernelState_rip));
	}

	// Any process ready?
	hlt();
	while(true);
}
