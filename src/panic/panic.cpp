#include "panic.hpp"
#include <klibc/klibc.hpp>

const char* const msgs[] = {
	"This is a test",
	"Limine didn't pass the memory map",
	"Empty memory map",
	"No memory to initialize the PMM",
	"Out of physical memory",
	"Too many regions in the memory map",
	"The program loader wasn't loaded as a stivale2 module",
	"The loaded loader.sus file is not an SUS file",
	"Paging::getPTE() received an address that wasn't present",
	"A bad size was given to construct an Allocator object",
	"A bad size was given to alloc() or free()",
	"A pure virtual function has been called inside the kernel",
	"Could not get an ASLR allocation",
	"MLvector received a data type bigger than PAGE_SIZE",
	"Unexpected interrupt",
	"Double fault, cannot recover",
	"Could not manage page fault",
	"Too many cores",
	"General Protection Fault",
	"No stdlib",
	"Error parsing stdlib",
	"Bootstrap ELF not found",
	"Bootstrap ELF is too big (>1GB)",
	"Error parsing bootstrap ELF",
	"Dispatcher returned",
	"ACPI RSDP has a bad signature",
	"ACPI RSDP has a bad checksum",
	"I live in a constant state of stress and pain",
	"No APIC detected",
	"Turns out APIC address was already in use",
	"APIC address is weird",

	"AHCI driver detected no devices.\n"
	"Strife does not support IDE, which is what you're probably using.\n"
	"If this is a VM, configure it to use AHCI instead of IDE.\n"
	"VirtualBox might also cause some problems. If it doesn't work, try qemu.",

	"Critical process died",
};

void panic(size_t id, bool doNotHalt) {
	setColor(0x4F);
	printf("PANIC!\n\n%s", msgs[id]);
	if(!doNotHalt) { hlt(); while(true); }
}

[[noreturn]] void hardPanic(size_t id) {
	panic(id);
	hlt();
	while(true);
}
