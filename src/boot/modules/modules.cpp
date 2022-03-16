#include "modules.hpp"
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <asm.hpp>
#include <panic/panic.hpp>
#include <mem/paging/paging.hpp>

uint64_t BootModules::begins[BootModules::NMODULES];
uint64_t BootModules::sizes[BootModules::NMODULES];

void BootModules::save(stivale2_struct* bootData) {
	// TODO strict aliasing violation?
	auto* modules = (stivale2_struct_tag_modules*)stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_MODULES_ID);

	stivale2_module* arr = modules->modules;
	size_t count = modules->module_count;
	for(size_t i=0; i<count; ++i) {
		size_t begin = arr[i].begin + HIGHER_HALF;
		size_t end = arr[i].end + HIGHER_HALF;
		size_t sz = end - begin;

		size_t code = arr[i].string[0] - '0'; // ugly atoi()
		if(code <= NMODULES) {
			begins[code] = begin;
			sizes[code] = sz;
		}
	}
}
