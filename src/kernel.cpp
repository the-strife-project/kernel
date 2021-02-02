#include <common.hpp>
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>
#include <GDT/MyGDT.hpp>

extern "C" void kmain(stivale2_struct*) {
	printf("Hold on to your seats, as jotaOS is booting");
	initGDT();
	while(true);
}
