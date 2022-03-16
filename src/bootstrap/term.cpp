#include "bootstrap.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/PIDs/PIDs.hpp>

void Bootstrap::term() {
	uint64_t beg = BootModules::begins[BootModules::MODULE_ID_TERM];
	uint64_t sz = BootModules::sizes[BootModules::MODULE_ID_TERM];
	if(!beg) {
		panic(Panic::NO_BOOTSTRAP, true);
		printf("\nBlame: term");
		hlt();
		while(true);
	}

	Loader::mapELF(beg, sz);

	Task* task = getTask(Loader::LOADER_PID).task;
	task->getState().regs.rax = sz;
	task->dispatchSaving();

	// Loader has finished its business
	if(Loader::last_err) {
		panic(Panic::BAD_BOOTSTRAP, true);
		printf("\nBlame: term\n");
		printf("Err: 0x%x\n", Loader::last_err);
		hlt();
		while(true);
	}

	getTask(Loader::last_pid).task->jump(Loader::last_entry);
}
