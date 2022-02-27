#include "modules.hpp"
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <asm.hpp>
#include <panic/panic.hpp>
#include <mem/paging/paging.hpp>

uint64_t stivale2Modules::loader_beg = 0;
uint64_t stivale2Modules::loader_end = 0;
uint64_t stivale2Modules::stdlib_beg = 0;
uint64_t stivale2Modules::stdlib_end = 0;

enum {
	MODULE_ID_LOADER,
	MODULE_ID_STDLIB
};

void stivale2Modules::save(stivale2_struct* bootData) {
	auto* modules = (stivale2_struct_tag_modules*)stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_MODULES_ID);

	stivale2_module* arr = modules->modules;
	size_t count = modules->module_count;
	for(size_t i=0; i<count; ++i) {
		size_t begin = arr[i].begin + HIGHER_HALF;
		size_t end = arr[i].end + HIGHER_HALF;

		switch(arr[i].string[0] - '0') {
		case MODULE_ID_LOADER:
			loader_beg = begin;
			loader_end = end;
			break;
		case MODULE_ID_STDLIB:
			stdlib_beg = begin;
			stdlib_end = end;
			break;
		}
	}

	if(!loader_beg) panic(Panic::NO_LOADER);
}
