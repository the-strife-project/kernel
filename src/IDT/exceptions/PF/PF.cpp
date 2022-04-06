#include <klibc/klibc.hpp>
#include <panic/panic.hpp>

struct PFErr {
	enum {
		P, W, U, R, I
	};
};

extern "C" void catchPF(uint32_t err) {
	// Some checks would go here
	// Remember that PF in ring 0 is not necessarily bad

	panic(Panic::UNKNOWN_PAGE_FAULT, true);
	printf("\nAt: 0x%x\n", getCR2());

	printf("Information:\n");

	if(err & (1 << PFErr::P))
		printf("Page protection violation.\n");
	else
		printf("Non-present page.\n");

	if(err & (1 << PFErr::W))
		printf("On write.\n");
	else
		printf("On read.\n");

	if(err & (1 << PFErr::U))
		printf("On ring 3. Should kill the process smh\n");

	if(err & (1 << PFErr::R))
		printf("Reserved bits to one. What did you do?!?!?!\n");

	if(err & (1 << PFErr::I))
		printf("Due to NX.\n");

	hlt();
}
