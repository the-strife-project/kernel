#include <boot/modules/modules.hpp>
#include <tasks/loader/parsers/USU/USU.hpp>
#include <tasks/loader/loader.hpp>
#include <tasks/task/task.hpp>

void Loader::startELFParser() {
	USU usu(stivale2Modules::elf_beg, stivale2Modules::elf_end);
	usu.parse();
	LoaderInfo load = Loader::load(usu.mappings);
	Task task(load, load.base + usu.entrypoint);
	// TODO: Send to the scheduler
	task.resume();
}
