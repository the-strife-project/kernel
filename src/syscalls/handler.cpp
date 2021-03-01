#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>

extern "C" void syscall_handler(size_t op, ...) {
	va_list args;
	va_start(args, op);

	switch(op) {
	case Syscalls::EXIT:
		printf("Should kill");
		hlt();
		break;
	case Syscalls::MORE_HEAP:
		printf("More heap!");
		hlt();
		break;
	default:
		// TODO: kill
		printf("Unknown syscall");
		hlt();
		break;
	}

	sysret();
	hlt(); while(true);
}
