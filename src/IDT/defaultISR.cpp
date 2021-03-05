#include <asm.hpp>
#include <panic/panic.hpp>
#include <klibc/klibc.hpp>

extern "C" void defaultISR(uint16_t intno) {
	panic(Panic::UNEXPECTED_INTERRUPT, true);
	printf("\nVector number: %x\n", intno);
	hlt(); while(true);
}
