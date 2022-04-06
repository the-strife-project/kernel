#include "loader.hpp"
#include "../PIDs/PIDs.hpp"

const size_t PF_X = 0x1;
const size_t PF_W = 0x2;
const size_t PF_R = 0x4;

// Handler for the MAP_IN syscall
bool Loader::mapIn(PID pid, uint64_t local, uint64_t remote) {
	size_t prot = remote & 0b111;
	remote &= ~0b111;

	// Get phys from loader
	Paging lp = getTask(LOADER_PID).task->getPaging();
	Paging rp = getTask(pid).task->getPaging();
	uint64_t phys = lp.getPhys(local);

	// Just make sure it's not zero
	if(!phys)
		return false;

	// Should be zero in remote
	if(rp.getPhys(remote))
		return false;

	// Map in remote, unmap in local
	size_t flags = 0;
	if(!(prot & PF_W)) flags |= Paging::MapFlag::RO;
	if(!(prot & PF_X)) flags |= Paging::MapFlag::NX;
	flags |= Paging::MapFlag::USER;

	rp.map(remote, phys, PAGE_SIZE, flags);
	lp.unmap(local);
	getTask(LOADER_PID).task->getASLR().free(local);

	return true;
}
