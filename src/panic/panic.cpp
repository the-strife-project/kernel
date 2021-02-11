#include "panic.hpp"
#include <klibc/klibc.hpp>

const char* const msgs[] = {
	"This is a test",
	"Limine didn't pass the memory map",
	"Empty memory map",
	"No memory to initialize the PMM",
	"Out of physical memory",
	"Too many regions in the memory map",
	"Tried to retract an extended PDE",
	"ELF parser wasn't loaded as a stivale2 module",
	"The loaded ELF.usu file is not an USU file",
	"Paging::getPTE() received an address that wasn't present",
	"A bad size was given to construct an Allocator object"
};

void panic(size_t id, bool doNotHalt) {
	setColor(0x4F);
	printf("PANIC!\n\n%s", msgs[id]);
	if(!doNotHalt) { hlt(); while(true); }
}
