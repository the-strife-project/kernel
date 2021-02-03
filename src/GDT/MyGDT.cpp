#include "GDT.hpp"
#include "MyGDT.hpp"

static uint64_t _gdt[GDT_N_ENTRIES];
static GDT gdt;

void initGDT() {
	gdt.setGDT(_gdt);

	// Null descriptor
	GDT::CoolDescriptor d0;
	gdt.addDescriptor(d0);

	// Kernel data
	GDT::CoolDescriptor d1;
	d1.setPresent();
	d1.setSystem();
	gdt.addDescriptor(d1);

	// Kernel code
	GDT::CoolDescriptor d2;
	d2.setPresent();
	d2.setSystem();
	d2.setCode();
	gdt.addDescriptor(d2);

	gdt.load();
}
