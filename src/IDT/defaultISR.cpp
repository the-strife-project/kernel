#include <klibc/klibc.hpp>
#include <asm.hpp>

extern "C" void defaultISR(uint16_t intno) {
	printf("\n[[[ UNEXPECTED INTERRUPT: %x ]]]\n", intno);
	hlt(); while(true);
}
