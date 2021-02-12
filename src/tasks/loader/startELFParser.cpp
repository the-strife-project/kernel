#include <boot/modules/modules.hpp>
#include <tasks/loader/parsers/USU/USU.hpp>
#include <tasks/loader/loader.hpp>

void Loader::startELFParser() {
	USU usu(stivale2Modules::elf_beg, stivale2Modules::elf_end);
	usu.parse();
	Loader::load(usu.mappings);
}
