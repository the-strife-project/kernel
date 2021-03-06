#include <boot/modules/modules.hpp>
#include <tasks/loader/parsers/USU/USU.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <tasks/PIDs/PIDs.hpp>

size_t ELF_PID;

void Loader::startELFParser() {
	USU usu(stivale2Modules::elf_beg, stivale2Modules::elf_end);
	usu.parse();
	LoaderInfo load = Loader::load(usu.mappings);

	Task* task = (Task*)PMM::calloc();
	*task = Task(load, load.base + usu.entrypoint);

	Scheduler::SchedulerTask schedTask;
	schedTask.paging = task->getPaging();
	schedTask.task = task;
	schedTask.paging.map((uint64_t)generalTask, (uint64_t)task, PAGE_SIZE, Paging::MapFlag::NX);

	PID pid = assignPID(schedTask);
	ELF_PID = pid;

	running[whoami()] = ELF_PID;

	schedTask.task->dispatchSaving();
	return;
}
