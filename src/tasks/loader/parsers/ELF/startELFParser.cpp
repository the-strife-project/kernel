#include <boot/modules/modules.hpp>
#include <tasks/loader/parsers/USU/USU.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <SMP/SMP.hpp>

size_t ELF_PID;

void Loader::startELFParser() {
	USU usu(stivale2Modules::elf_beg, stivale2Modules::elf_end);
	usu.parse();
	LoaderInfo load = Loader::load(usu.mappings);

	Task* task = (Task*)PMM::calloc();
	*task = Task(load, load.base + usu.entrypoint);

	Scheduler::SchedulerTask schedTask;
	schedTask.paging = task->getPaging();
	schedTask.paging.map((uint64_t)generalTask, (uint64_t)task, PAGE_SIZE, Paging::MapFlag::NX);

	PID pid = assignPID(schedTask);
	ELF_PID = pid;

	running[BSP] = ELF_PID;
	schedTask.paging.load();
	generalTask->resume();
}
