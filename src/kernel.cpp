#include <common.hpp>
#include <boot/stivale2.h>

void write(const char* str) {
	char* video = (char*)0xB8000;

	while(*str) {
		*video++ = *str++;
		*video++ = 0x07;
	}
}

extern "C" void kmain(stivale2_struct*) {
	write("Hello world from Limine :)");
	while(true);
}
