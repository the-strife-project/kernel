#include "IPC.hpp"
#include <tasks/PIDs/PIDs.hpp>
#include <klibc/ht64/ht64.hpp>
#include <kkill>

static HT64 psns;
static Spinlock lock;

void IPC::initPSNS() {
	// PSNS needs some space in virtual addressing
	psns = HT64(GLOBAL_PSNS_REGION, GLOBAL_PSNS_SIZE);
	lock = Spinlock();
}

bool IPC::publish(PID pid, uint64_t name) {
	// TODO Did this process publish already?

	if(!name)
		getMyCurrent().task->kill(std::kkill::BAD_STRING);

	lock.acquire();
	if(psns.has(name)) {
		lock.release();
		return false;
	}

	psns.add(name, pid);
	lock.release();
	return true;
}

PID IPC::resolve(uint64_t name) {
	lock.acquire();
	PID ret = psns.get(name);
	lock.release();
	return ret;
}