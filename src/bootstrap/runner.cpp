#include "bootstrap.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/PIDs/PIDs.hpp>

// Common function for all critical processes

PID Bootstrap::run(const char* name, size_t moduleID) {
	printf("* %s ", name);
	uint64_t beg = BootModules::begins[moduleID];
	uint64_t sz = BootModules::sizes[moduleID];
	if(!beg) {
		panic(Panic::NO_BOOTSTRAP, true);
		printf("\nBlame: %s", name);
		hlt();
		while(true);
	}

	Loader::mapELF(beg, sz);

	Task* loader = getTask(Loader::LOADER_PID).task;
	loader->getState().regs.rax = sz;
	loader->dispatchSaving();

	// Loader has finished its business
	if(Loader::last_err) {
		panic(Panic::BAD_BOOTSTRAP, true);
		printf("\nBlame: %s\n", name);
		printf("Error: 0x%x\n", Loader::last_err);
		hlt();
		while(true);
	}

	Task* task = getTask(Loader::last_pid).task;
	task->jump(Loader::last_entry); // Set RIP

	return Loader::last_pid;
}
