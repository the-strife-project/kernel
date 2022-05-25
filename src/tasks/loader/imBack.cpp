#include "loader.hpp"
#include <tasks/scheduler/scheduler.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <CPU/SMP/SMP.hpp>

PID Loader::last_pid;
size_t Loader::last_err;
uint64_t Loader::last_entry;

void Loader::imBack(PID pid, size_t err, uint64_t entry) {
	last_pid = pid;
	last_err = err;
	last_entry = entry;

	// This is a syscall, so lock is acquired
	Task* task = getTask(Loader::LOADER_PID).get()->task;

	// Save state to task
	task->saveStateSyscall();
}
