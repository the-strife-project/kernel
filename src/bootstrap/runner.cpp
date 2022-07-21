#include "bootstrap.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>

// Common function for all critical processes

PID Bootstrap::run(const char* name, size_t moduleID, bool noprint) {
	if(!noprint)
		printf("* %s ", name);

	uint64_t beg = BootModules::begins[moduleID];
	uint64_t sz = BootModules::sizes[moduleID];
	if(!beg) {
		panic(Panic::NO_BOOTSTRAP, true);
		printf("\nBlame: %s", name);
		hlt();
		while(true);
	}

	auto pp = getTask(Loader::LOADER_PID);
	pp.acquire();
	Loader::mapELF(beg, sz);

	Task* loader = pp.get()->task;
	loader->getState().regs.rax = sz;

	setOrigRunning(Loader::LOADER_PID);
	pp.release();
	loader->dispatchSaving();
	pp.acquire();

	// Loader has finished its business
	if(Loader::last_err) {
		panic(Panic::BAD_BOOTSTRAP, true);
		printf("\nBlame: %s\n", name);
		printf("Error: 0x%x\n", Loader::last_err);
		hlt();
		while(true);
	}

	Loader::freeELF();
	PID ret = Loader::last_pid;
	setOrigRunning(NULL_PID);
	pp.release();

	pp = getTask(ret);
	pp.acquire();
	pp.get()->uid = UID_SYSTEM;
	pp.get()->task->jump(Loader::last_entry); // Set RIP
	pp.release();

	return ret;
}
