#include "loader.hpp"
#include <tasks/scheduler/scheduler.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>

size_t Loader::last_err;
uint64_t Loader::last_entry;

void Loader::imBack(size_t err, uint64_t entry) {
	last_err = err;
	last_entry = entry;

	// Save state to task
	Task* task = getTask(running[whoami()]).task;
	task->putState(savedState[whoami()]);

	// Resume saved state from kernel
	asmRestoreKernel();
}
