#include <klibc/klibc.hpp>
#include <panic/panic.hpp>

extern "C" void catchGPF(size_t err, size_t addr) {
	panic(Panic::GENERAL_PROTECTION_FAULT, true);
	printf("\n");
	if(err)
		printf("Segment: 0x%x\n", err);
	printf("Caused by: 0x%x\n", addr);
	hlt();
}
