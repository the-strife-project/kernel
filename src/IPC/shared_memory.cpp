#include "IPC.hpp"
#include <rng/rng.hpp>
#include <tasks/PIDs/PIDs.hpp>

static Task::SharedSegment getSM(Task* target, std::SMID smid, bool create=false) {
	auto* shared = target->getShared();
	if(!shared && !create)
		return Task::SharedSegment();

	if(!shared) {
		shared = (Task::SharedSegment*)PhysMM::calloc();
		target->setShared(shared);
		return Task::SharedSegment();
	}

	for(size_t i=0; i<Task::NUM_SHARED_SEGMENTS; ++i)
		if(shared[i].smid == smid)
			return shared[i];

	return Task::SharedSegment();
}

static uint8_t newSM(Task* target, std::SMID smid, uint64_t kptr, size_t npages) {
	if(getSM(target, smid, true).smid)
		return 0; // Already exists

	auto* shared = target->getShared();
	for(size_t i=0; i<Task::NUM_SHARED_SEGMENTS; ++i) {
		if(!shared[i].smid) {
			// Free slot!
			shared[i].smid = smid;
			shared[i].kptr = kptr;
			shared[i].npages = npages;
			shared[i].allowed = 0;
			return 1; // OK
		}
	}

	// No free slots :(
	return 2;
};

static const size_t SHARED_MEMORY_MAX_PAGES = 1 << 11;
std::SMID IPC::smMake(Task* me, size_t npages) {
	uint64_t kptr = PhysMM::calloc(npages);
	if(!kptr)
		return 0;

	while(true) {
		std::SMID ret = getRandom64();

		auto err = newSM(me, ret, kptr, npages);
		if(err == 1) {
			// PhysMM::alloc() sets counter to zero
			return ret; // OK
		} else if(err == 2)
			break; // No free slots
	}

	PhysMM::free(kptr, npages);
	return 0;
}

bool IPC::smAllow(Task* me, std::SMID smid, PID pid) {
	auto sm = getSM(me, smid);
	if(!sm.smid)
		return false;

	// Already allowed?
	if(sm.allowed)
		return false;

	auto* shared = me->getShared();
	for(size_t i=0; i<Task::NUM_SHARED_SEGMENTS; ++i) {
		if(shared[i].smid == smid) {
			shared[i].allowed = pid;
			return true;
		}
	}

	// Can't happen
	bruh(Bruh::SM_ALLOW_IMPOSSIBLE);
	return false;
}

bool IPC::smRequest(Task* me, PID myself, PID pid, std::SMID smid) {
	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		pp.release();
		return false;
	}

	Task* other = pp.get()->task;
	auto sm = getSM(other, smid);

	// Can release now
	pp.release();

	if(!sm.smid)
		return false; // Not found in client's SMIDs

	if(sm.allowed != myself)
		return false; // Not allowed :(

	// Looks good to me
	auto err = newSM(me, smid, sm.kptr, sm.npages);
	return err == 1;
}

static const uint64_t mapflags = Paging::MapFlag::NX | Paging::MapFlag::USER;
uint64_t IPC::smMap(Task* me, std::SMID smid) {
	auto sm = getSM(me, smid);
	if(!sm.smid)
		return 0;

	uint64_t tptr = me->getASLR().get(sm.npages, GROWS_UPWARD, PAGE_SIZE, DO_NOT_PANIC);
	if(!tptr)
		return 0;

	me->getPaging().map(tptr, sm.kptr, PAGE_SIZE * sm.npages, mapflags);
	for(size_t i=0; i<sm.npages; ++i)
		kpaging.getPTE(sm.kptr + i * PAGE_SIZE)->incUsedChunks();

	me->incUsedPages(sm.npages);
	return tptr;
}

size_t IPC::smGetSize(Task* me, std::SMID smid) {
	auto sm = getSM(me, smid);
	if(!sm.smid)
		return 0;
	return sm.npages;
}

void IPC::smDrop(Task* me, std::SMID smid) {
	auto* shared = me->getShared();
	for(size_t i=0; i<Task::NUM_SHARED_SEGMENTS; ++i) {
		if(shared[i].smid == smid) {
			shared[i].smid = 0;
			return;
		}
	}
}
