#include "task.hpp"

extern "C" void actuallyResumeTask(uint64_t rsp, uint64_t rip, GeneralRegisters*, uint64_t rflags);

void Task::resume() {
	paging.reload();
	actuallyResumeTask(rsp, rip, &regs, rflags);
}
