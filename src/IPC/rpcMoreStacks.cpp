#include "IPC.hpp"
#include <tasks/task/task.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>
#include <kkill>

uint64_t IPC::rpcMoreStacks(PID remote) {
	// rpcSwitcher acquires it
	auto pp = getTask(remote);

	Task* task = pp.get()->task;
	uint64_t* rpcStacks = task->getRPCStacks();

	// Time to allocate a new stack
	size_t ret = 0;
	for(size_t i=0; i<256; ++i) {
		if(rpcStacks[i] == 1) {
			rpcStacks[i] = 0; // In use
			// Grab an ASLR region. Aligned to PAGE_SIZE for simplicity.
			ret = task->getASLR().get(MAX_STACK_PAGES, GROWS_DOWNWARD,
									   PAGE_SIZE, DO_NOT_PANIC);
			if(!ret)
				break;

			// Allocate first page, kernel only. Contains return ticket
			uint64_t virt = PAGE(ret);
			virt -= PAGE_SIZE;
			uint64_t page = PhysMM::calloc();
			size_t flags = 0;
			flags |= Paging::MapFlag::NX;
			//flags |= Paging::MapFlag::USER;
			task->getPaging().map(virt, page, PAGE_SIZE, flags);

			// Second page, user
			virt -= PAGE_SIZE;
			page = PhysMM::calloc();
			flags |= Paging::MapFlag::USER;
			task->getPaging().map(virt, page, PAGE_SIZE, flags);
			break;
		}
	}

	return ret;
}
