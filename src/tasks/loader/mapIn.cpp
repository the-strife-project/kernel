#include "loader.hpp"
#include "../PIDs/PIDs.hpp"

// Handler for the MAP_IN syscall
bool Loader::mapIn(PID pid, uint64_t local, uint64_t remote) {
	size_t prot = remote & 0b11;
	remote &= ~0b11;

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
	if(!(prot & 0b01)) flags |= Paging::MapFlag::RO;
	if(!(prot & 0b10)) flags |= Paging::MapFlag::NX;
	flags |= Paging::MapFlag::USER;

	rp.map(remote, phys, PAGE_SIZE, flags);
	lp.unmap(local);

	return true;
}
