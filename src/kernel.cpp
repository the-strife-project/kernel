#include <common.hpp>
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <GDT/MyGDT.hpp>
#include <IDT/MyIDT.hpp>
#include <asm.hpp>

extern "C" void kmain(stivale2_struct*) {
	printf("Hold on to your seats, as jotaOS is booting\n\n");

	initGDT();
	initIDT();

	asm volatile("int $0x80");
	hlt(); while(true);
}
