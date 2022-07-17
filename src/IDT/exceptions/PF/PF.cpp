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
	// First check: RPC return
	if(err & (1 << PFErr::U)) {
		// Caused by user
		if(!(getCR2() >> 63)) { // Lower half
			if(err & (1 << PFErr::P)) { // Page protection
				if(!PAGEOFF(getCR2())) { // Fault at page beginning
					// That's rpcReturn!
					asm volatile("mov %%rbp, %%rdi\n"
								 "mov %%cr2, %%rsp\n"
								 "jmp rpcReturn"
								 :: "d" (rax));
				}
			}
		}

		// TODO: Check more stack here

		// Unrecognized page fault by user
		printf("Unrecognized page fault in userland. Panicking for debugging purposes.");
		//exceptionKill(std::kkill::SEGFAULT);
	}
	// Some checks would go here
	// Remember that PF in ring 0 is not necessarily bad

	panic(Panic::UNKNOWN_PAGE_FAULT, true);
	printf("\nFor: 0x%x\n", getCR2());

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

	// This is debug
	if(err & (1 << PFErr::U)) {
		auto core = whoami();
		printf("That was %x as %x\n", origRunning[core], runningAs[core]);
		auto pp = getTask(runningAs[core]);
		pp.acquire();
		if(pp.isNull()) {
			printf("NULL!?!?!?!?!\n");
			hlt();
		}

		printf("ASLR dump:\n");
		for(auto const& x : pp.get()->task->getASLR().getIDs())
			printf("%x - %x\n", x.f, x.s);

		pp.release();
		exceptionKill(std::kkill::SEGFAULT);
	}

	hlt();
}
