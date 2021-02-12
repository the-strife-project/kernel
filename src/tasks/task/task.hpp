#ifndef TASK_HPP
#define TASK_HPP

#include <klibc/klibc.hpp>

/*
	Tasks will be in public dynamic memory, that is, always present,
	and not allocated temporarily, they're freed when the process dies.

	For this, and as there might be a lot of tasks, the size of this class
	in memory must be a power of two so no memory is wasted.
*/

// Registers pushed onto the stack when an interrupt happens
struct GeneralRegisters {
	uint64_t rax = 0;
	uint64_t rbx = 0;
	uint64_t rcx = 0;
	uint64_t rdx = 0;
	uint64_t rsi = 0;
	uint64_t rdi = 0;
	uint64_t rbp = 0;
	uint64_t r8 = 0;
	uint64_t r9 = 0;
	uint64_t r10 = 0;
	uint64_t r11 = 0;
	uint64_t r12 = 0;
	uint64_t r13 = 0;
	uint64_t r14 = 0;
	uint64_t r15 = 0;
} __attribute__((packed));

class Task {
private:
	uint64_t cr3;
	GeneralRegisters regs;
	uint64_t rsp, rsi;

public:
	inline Task() {}
	inline Task(Paging paging, uint64_t entrypoint)
		: cr3((uint64_t)(paging.getData())), rsi(entrypoint)
	{}

	void resume();
};

#endif
