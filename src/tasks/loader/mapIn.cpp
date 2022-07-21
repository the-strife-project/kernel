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
	auto ppl = getTask(LOADER_PID); // Acquired already
	Task* ltask = ppl.get()->task;
	Paging lp = ppl.get()->paging;

	auto ppr = getTask(pid);
	ppr.acquire();
	if(ppr.isNull()) {
		ppr.release();
		return false;
	}
	Paging rp = ppr.get()->paging;

	uint64_t phys = lp.getPhys(local);

	// Just make sure it's not zero
	if(!phys) {
		ppr.release();
		return false;
	}

	// Should be zero in remote
	if(rp.getPhys(remote)) {
		ppr.release();
		return false;
	}

	// Map in remote, unmap in local
	size_t flags = 0;
	if(!(prot & PF_W)) flags |= Paging::MapFlag::RO;
	if(!(prot & PF_X)) flags |= Paging::MapFlag::NX;
	flags |= Paging::MapFlag::USER;

	rp.map(remote, phys, PAGE_SIZE, flags);
	ppr.get()->task->incUsedPages();
	ppl.get()->task->decUsedPages();
	ppr.release();

	lp.unmap(local);
	ltask->getASLR().free(local);

	return true;
}
