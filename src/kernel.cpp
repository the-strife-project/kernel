#include <klibc/klibc.hpp>
#include <panic/panic.hpp>
#include <boot/modules/modules.hpp>
#include <GDT/MyGDT.hpp>
#include <IDT/MyIDT.hpp>
#include <drivers/PIC8259/PIC8259.hpp>
#include <mem/memmap/memmap.hpp>
#include <mem/PMM/PMM.hpp>
#include <mem/paging/paging.hpp>
#include <tasks/loader/loader.hpp>

__attribute__((section(".memmap"), used))
stivale2_mmap_entry savedmemmap[PAGE_SIZE / sizeof(stivale2_mmap_entry)];

extern "C" void kmain(stivale2_struct* bootData) {
	printf("Hold on to your seats, as jotaOS is booting\n\n");

	MemoryMap memmap(bootData);
	if(memmap.empty())
		panic(Panic::EMPTY_MEMORY_MAP);
	else if(memmap.getn() > 170)
		panic(Panic::TOO_MANY_REGIONS);

	// Move the memory map somewhere safe
	memmap.move(savedmemmap);

	// Now, save the modules (next commit)
	stivale2Modules::save(bootData);

	printf("Setting GDT... "); initGDT(); printf("[OK]\n");
	printf("Setting IDT... "); initIDT(); printf("[OK]\n");
	printf("Initializing PMM... "); PMM::init(memmap); printf("[OK]\n");
	printf("Paging memory... "); initKernelPaging(memmap); printf("[OK]\n");
	PMM::finalizeInit(memmap);
	initAllocators();

	PubList<int> asdf;
	for(int i=0; i<1000; ++i)
		asdf.push_back(i);
	asdf.clear();

	printf("Loading ELF parser... "); Loader::startELFParser(); printf("[OK]\n");

	printf("\nThat's all for now folks.");

	hlt(); while(true);
}
