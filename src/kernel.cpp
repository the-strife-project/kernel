#include <klibc/klibc.hpp>
#include <GDT/MyGDT.hpp>
#include <IDT/MyIDT.hpp>
#include <asm.hpp>
#include <drivers/PIC8259/PIC8259.hpp>
#include <mem/memmap/memmap.hpp>
#include <mem/PMM/PMM.hpp>

extern "C" void kmain(stivale2_struct* bootData) {
	printf("Hold on to your seats, as jotaOS is booting\n\n");

	MemoryMap memmap(bootData);
	if(memmap.empty()) {
		// TODO: Actually panic
		printf("PANIC: Memory map is empty");
		hlt(); while(true);
	}

	printf("Setting GDT... ");
	initGDT();
	printf(":)\n");

	printf("Setting IDT... ");
	initIDT();
	printf(":)\n");

	printf("Booting up the PIC... ");
	PIC::init();
	printf(":)\n");

	printf("Initializing PMM... ");
	PMM::init(memmap);
	printf(":)\n");

	//printf("Paging... ");

	//PIC::up(0);

	hlt(); while(true);
}
