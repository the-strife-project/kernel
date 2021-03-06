#include <klibc/klibc.hpp>
#include <panic/panic.hpp>
#include <boot/modules/modules.hpp>
#include <GDT/MyGDT.hpp>
#include <IDT/MyIDT.hpp>
#include <mem/memmap/memmap.hpp>
#include <mem/PMM/PMM.hpp>
#include <mem/paging/paging.hpp>
#include <CPU/TSS/TSS.hpp>
#include <syscalls/syscalls.hpp>
#include <mem/stacks/stacks.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>

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

	printf("Setting descriptors... ");
	initGDT();
	initIDT();
	printf("[OK]\n");

	printf("Doing a lot of memory stuff... ");
	PMM::init(memmap);
	initKernelPaging(memmap);
	PMM::finalizeInit(memmap);	// Bootloader pages are now free to use.
	initAllocators();

	ncores = 1;
	if(ncores > 512)
		panic(Panic::TOO_MANY_CORES);

	prepareStacks(ncores);
	printf("[OK]\n");

	printf("Setting up TSS... ");
	for(size_t i=0; i<ncores; ++i) {
		// This possibly isn't done like this in SMP. Just a stub
		TSS tss = newTSS();
		tss.setRSP0(pubStacks[i]);
		tss.setIST(IST_PAGE_FAULT, VMM::Public::alloc());
		tss.setIST(IST_DOUBLE_FAULT, VMM::Public::alloc());
		tss.load();
	}

	printf("[OK]\n");

	initScheduler();
	enableSyscalls();

	printf("Loading ELF parser... "); Loader::startELFParser(); printf("[OK]\n");

	printf("\nThat's all for now folks.");

	hlt(); while(true);
}
