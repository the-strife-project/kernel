#include "GDT.hpp"
#include "MyGDT.hpp"

static uint64_t _gdt[32];	// Just a random number, enough to fit all TSSs
static GDT gdt;

void initGDT() {
	gdt.setGDT(_gdt);

	// Null descriptor
	GDT::CoolDescriptor d0;
	gdt.addDescriptor(d0);

	// Kernel data
	GDT::CoolDescriptor d1;
	d1.setPresent();
	d1.setUser();
	gdt.addDescriptor(d1);

	// Kernel code
	GDT::CoolDescriptor d2;
	d2.setPresent();
	d2.setUser();
	d2.setCode();
	gdt.addDescriptor(d2);

	// User data
	GDT::CoolDescriptor d3;
	d3.setPresent();
	d3.setUser();
	d3.setRing3();
	gdt.addDescriptor(d3);

	// User code
	GDT::CoolDescriptor d4;
	d4.setPresent();
	d4.setUser();
	d4.setCode();
	d4.setRing3();
	gdt.addDescriptor(d4);

	gdt.load();
}

uint16_t newTSSsegment(uint64_t addr) {
	GDT::CoolTSSDescriptor desc;
	desc.base = addr;
	gdt.addTSS(desc);
	gdt.load();
	return (gdt.getctr()-2)*GDT_LAME_DESCRIPTOR_SIZE;
}
