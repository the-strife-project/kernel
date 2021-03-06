#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <GDT/MyGDT.hpp>

extern "C" void asmSyscallHandler();
void enableSyscalls() {
	uint64_t efer = rdmsr(MSR_EFER);
	efer |= 1 << EFER_SYSCALL_ENABLE;
	wrmsr(MSR_EFER, efer);

	wrmsr(MSR_LSTAR, (uint64_t)asmSyscallHandler);

	uint64_t star = 0;
	star |= (((uint64_t)SEGMENT_SELECTOR_UDATA - 0x08) | USER_PL) << STAR_SYSRET_SELECTOR;
	star |= ((uint64_t)SEGMENT_SELECTOR_KCODE) << STAR_SYSCALL_SELECTOR;
	wrmsr(MSR_STAR, star);
}
