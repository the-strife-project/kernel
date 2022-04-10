#include <klibc/klibc.hpp>
#include <panic/panic.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <kkill>

struct PFErr {
	enum {
		P, W, U, R, I
	};
};

#include <CPU/SMP/SMP.hpp>
extern "C" void catchPF(size_t err, uint64_t iretqs) {
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

	if(err & (1 << PFErr::U))
		getMyCurrent().task->kill(std::kkill::SEGFAULT);

	hlt();
}
