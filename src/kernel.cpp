#include <common.hpp>
#include <boot/stivale2.h>
#include <klibc/klibc.hpp>

extern "C" void kmain(stivale2_struct*) {
	printf("Hold on to your seats, jotaOS is booting");
	while(true);
}
