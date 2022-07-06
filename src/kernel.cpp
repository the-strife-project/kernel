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
#include <bootstrap/bootstrap.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <IPC/IPC.hpp>
#include <drivers/ACPI/ACPI.hpp>
#include <drivers/APIC/APIC.hpp>
#include <CPU/sec/sec.hpp>
#include <rng/rng.hpp>

__attribute__((section(".memmap"), used))
stivale2_mmap_entry savedmemmap[PAGE_SIZE / sizeof(stivale2_mmap_entry)];

extern "C" void kmain(stivale2_struct* bootData) {
	printf("Hold on to your seats, as Strife is booting\n\n");

	MemoryMap memmap(bootData);
	if(memmap.empty())
		panic(Panic::EMPTY_MEMORY_MAP);
	else if(memmap.getn() > 170)
		panic(Panic::TOO_MANY_REGIONS);

	// Move the memory map somewhere safe
	memmap.move(savedmemmap);

	// Now, save the modules
	BootModules::save(bootData);

	printf("Setting descriptors... ");
	initGDT();
	initIDT();
	printf("[OK]\n");

	printf("First memory initialization... ");
	PMM::init(memmap);
	initKernelPaging(memmap);
	printf("[OK]\n");

	printf("Parsing ACPI tables... ");
	ACPI::parse(bootData);
	printf("[OK]\n");

	printf("Finishing memory... ");
	PMM::finalizeInit(memmap);	// Bootloader pages are now free to use.
	initAllocators();

	ncores = 1;
	if(ncores > MAX_CORES)
		panic(Panic::TOO_MANY_CORES);

	prepareStacks(ncores);
	printf("[OK]\n");

	printf("Initializing APIC... ");
	APIC::init();
	APIC::initTimer();
	// IOAPIC here
	printf("[OK]\n");

	for(size_t i=0; i<ncores; ++i) {
		// This possibly isn't done like this in SMP. Just a stub
		TSS tss = newTSS();
		tss.setRSP0(pubStacks[i]);
		for(size_t i=1; i<=N_ISTS; ++i)
			tss.setIST(i, VMM::Public::alloc() + PAGE_SIZE);
		tss.load();
	}

	initScheduler();
	enableSyscalls();

	Security::enableSMEP();
	Security::enableSMAP();

	printf("Gathering entropy... ");
	initRandom();
	printf("[OK]\n");

	printf("Bootstrapping the loader... ");
	Loader::bootstrapLoader();
	printf("[OK]\n");

	// Now it's time to set up the userspace
	printf("\n - Bootstrapping userspace - \n");
	Bootstrap::bootstrap();

	// Bootstrapping is done. Time to fire up the rest of the cores.

	schedule();
}
