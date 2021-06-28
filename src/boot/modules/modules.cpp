#include "modules.hpp"
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <asm.hpp>
#include <panic/panic.hpp>
#include <mem/paging/paging.hpp>

uint64_t stivale2Modules::loader_beg = 0;
uint64_t stivale2Modules::loader_end = 0;

#define MODULE_ID_LOADER 0

void stivale2Modules::save(stivale2_struct* bootData) {
	auto* modules = (stivale2_struct_tag_modules*)stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_MODULES_ID);

	stivale2_module* current = modules->modules;
	uint64_t count = modules->module_count;
	while(count--) {
		switch(current->string[0] - '0') {
		case MODULE_ID_LOADER:
			loader_beg = current->begin + HIGHER_HALF;
			loader_end = current->end + HIGHER_HALF;
			break;
		}
	}

	if(!loader_beg) panic(Panic::NO_LOADER);
}
