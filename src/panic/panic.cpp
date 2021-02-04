#include "panic.hpp"

const char* const msgs[] = {
	"This is a test",
	"Limine didn't pass the memory map",
	"Empty memory map",
	"No memory to initialize the PMM",
	"Out of physical memory"
};

void panic(size_t id, bool doNotHalt) {
	setColor(0x4F);
	printf("PANIC!\n\n%s", msgs[id]);

	if(!doNotHalt) { hlt(); while(true); }
}
