#include <klibc/klibc.hpp>
#include <panic/panic.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <kkill>
#include <tasks/PIDs/PIDs.hpp>
#include "../exceptions.hpp"

struct PFErr {
	enum {
		P, W, U, R, I
	};
};

extern "C" void catchPF(size_t err, uint64_t iretqs, size_t rax) {
	// First check: caused by user, at lower half, in kernel-only
	if(err & (1 << PFErr::U)) { // Caused by user
		if(!(getCR2() >> 63)) { // Lower half
			if(err & (1 << PFErr::P)) { // Page protection
				if(!(getCR2() & 0xFFF)) { // Fault at page beginning
					// That's rpcReturn!
					asm volatile("mov %%rbp, %%rdi\n"
								 "mov %%cr2, %%rsp\n"
								 "jmp rpcReturn"
								 :: "d" (rax));
				}
			}
		}
	}
	// Some checks would go here
	// Remember that PF in ring 0 is not necessarily bad

	panic(Panic::UNKNOWN_PAGE_FAULT, true);
	printf("\nAt: 0x%x\n", getCR2());

	// Top of iretq struct has rip
	uint64_t rip = *(uint64_t*)iretqs;
	printf("RIP: 0x%x\n", rip);

	printf("Information:\n");

	if(err & (1 << PFErr::P))
		printf("Page protection violation.\n");
	else
		printf("Non-present page.\n");

	if(err & (1 << PFErr::W))
		printf("On write.\n");
	else
		printf("On read.\n");

	if(err & (1 << PFErr::R))
		printf("Reserved bits to one. What did you do?!?!?!\n");

	if(err & (1 << PFErr::I))
		printf("Due to NX.\n");

	// Time to return to the kernel
	kpaging.load();

	// TODO This check would need to be at the top of the function.
	if(err & (1 << PFErr::U)) {
		// PF from userspace. No biggie.
		// Assume segfault by now.
		exceptionKill(std::kkill::SEGFAULT);
	}

	hlt();
}
