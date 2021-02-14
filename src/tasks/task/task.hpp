#ifndef TASK_HPP
#define TASK_HPP

#include <klibc/klibc.hpp>
#include "registers.hpp"

/*
	Tasks will be in public dynamic memory, that is, always present,
	and not allocated temporarily, they're freed when the process dies.

	For this, and as there might be a lot of tasks, the size of this class
	in memory must be a power of two so no memory is wasted.
*/

class Task {
private:
	Paging paging;
	GeneralRegisters regs;
	uint64_t rip, rsp;
	uint64_t rflags = BASIC_RFLAGS;

public:
	inline Task() {}
	inline Task(Paging paging, uint64_t entrypoint, uint64_t stack)
		: paging(paging), rip(entrypoint), rsp(stack)
	{}

	void resume();
};

#endif
