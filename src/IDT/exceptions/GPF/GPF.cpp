#include <klibc/klibc.hpp>
#include <panic/panic.hpp>

extern "C" void catchGPF(uint32_t err) {
	// Some checks would go here
	asm volatile("xchgw %bx, %bx\nnop");

	panic(Panic::GENERAL_PROTECTION_FAULT, true);
	if(err)
		printf("\nSegment: 0x%x\n", err);
	hlt();
}
