#include "task.hpp"

void Task::destroy() {
	paging.destroy();

	// rpcStacks are freed with paging too

	if(shared) {
		PhysMM::free((uint64_t)shared, 1);
		shared = nullptr;
	}
}
