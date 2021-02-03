#include <common.hpp>
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <GDT/MyGDT.hpp>
#include <IDT/MyIDT.hpp>
#include <asm.hpp>
#include <drivers/PIC8259/PIC8259.hpp>

extern "C" void kmain(stivale2_struct*) {
	printf("Hold on to your seats, as jotaOS is booting\n\n");

	printf("Setting GDT... ");
	initGDT();
	printf(":)\n");

	printf("Setting IDT... ");
	initIDT();
	printf(":)\n");

	printf("Setting PIC... ");
	PIC::init();
	printf(":)\n");

	PIC::up(0);

	hlt(); while(true);
}
