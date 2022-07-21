#include "../syscalls.hpp"
#include <tasks/PIDs/PIDs.hpp>

void taskInfo(PID me, Task* mytask, PID pid, uint64_t page) {
	if(PAGEOFF(page))
		return; // Not page-aligned!

	auto phys = mytask->getPaging().getPhys(page);
	if(!phys)
		return; // Invalid page

	// At this point, page is valid, clear it
	auto* info = (std::Info*)phys;
	*info = std::Info();

	// Is PID 0?
	if(!pid) {
		// Set information regarding the kernel
		info->pid = info->uid = 0;
		info->mem = PhysMM::getUsedPages() * PAGE_SIZE;
		return;
	}

	auto pp = getTask(pid);
	if(pid != me) {
		pp.acquire();
	} else if(pp.isNull()) {
		pp.release();
		return; // Oops
	}

	// PID is fine
	auto stask = *(pp.get());
	info->pid = pid;
	info->uid = stask.uid;
	info->mem = stask.task->getUsedPages() * PAGE_SIZE;

	if(pid != me)
		pp.release();
}
