#include "../syscalls.hpp"
#include <CPU/SMP/SMP.hpp>

static const size_t r11Offset = sizeof(uint64_t)*(2 + 10);

size_t getIO(std::PID pid, Task* task) {
	// TODO Do the check here
	IGNORE(pid);

	uint64_t ss = (uint64_t)savedState[whoami()];
	uint64_t r11 = ss + r11Offset;

	// Force alignment, if not, well, good luck
	if(r11 % 8)
		return 2;

	// Now that it surely is in one page, get the physical
	uint64_t phys = task->getPaging().getPhys(r11);
	uint64_t* rflags = (uint64_t*)(phys + PAGEOFF(r11));

	// Set IOPL=3
	*rflags |= (1 << RFLAGS::IOPL0) | (1 << RFLAGS::IOPL1);
	return 0;
}
