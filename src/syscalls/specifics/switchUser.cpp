#include "../syscalls.hpp"
#include <tasks/PIDs/PIDs.hpp>

bool switchUser(PID pid, size_t uid) {
	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		pp.release();
		return false;
	}

	auto& stask = *(pp.get());
	stask.uid = uid;

	pp.release();
	return true;
}
