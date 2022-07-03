#include "modules.hpp"
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <asm.hpp>
#include <panic/panic.hpp>
#include <mem/paging/paging.hpp>

uint64_t BootModules::begins[BootModules::NMODULES];
uint64_t BootModules::sizes[BootModules::NMODULES];

// Take into account: the pointers to the modules are stored in regions
//   labeled as "KERNEL_AND_MODULES" in the memory map given by the bootloader.
// They are not USABLE or BOOTLOADER_RECLAIMABLE, so their addresses are never
//   going to be used for allocation in the PMM. It's safe to refer to them
//   at any point without backing them up.

void BootModules::save(stivale2_struct* bootData) {
	// TODO strict aliasing violation?
	auto* modules = (stivale2_struct_tag_modules*)stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_MODULES_ID);

	stivale2_module* arr = modules->modules;
	size_t count = modules->module_count;
	for(size_t i=0; i<count; ++i) {
		size_t begin = arr[i].begin + HIGHER_HALF;
		size_t end = arr[i].end + HIGHER_HALF;
		size_t sz = end - begin;

		// ugly atoi()
		size_t code = arr[i].string[0] - '0';
		code *= 10;
		code += arr[i].string[1] - '0';
		if(code <= NMODULES) {
			begins[code] = begin;
			sizes[code] = sz;
		}
	}
}
