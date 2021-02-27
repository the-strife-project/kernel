#ifndef SYSCALLS_HPP
#define SYSCALLS_HPP

#define MSR_EFER  0xC0000080
#define MSR_STAR  0xC0000081
#define MSR_LSTAR 0xC0000082

#define EFER_SYSCALL_ENABLE 0

#define STAR_SYSCALL_SELECTOR 32
#define STAR_SYSRET_SELECTOR  48

#include <stdarg.h>
#include <common.hpp>

struct Syscalls {
	enum {
		EXIT,
		MORE_HEAP
	};
};

void syscall_handler(size_t op, ...);
void enableSyscalls();

#endif
