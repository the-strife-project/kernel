#include "ELF.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/loader/USU/USU.hpp>

void ELF::startParser() {
	USU usu(stivale2Modules::elf_beg, stivale2Modules::elf_end);
}
