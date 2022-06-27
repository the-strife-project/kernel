#include "GDT.hpp"
#include "MyGDT.hpp"

static uint64_t _gdt[32];	// Just a random number, enough to fit all TSSs
static GDT gdt;

void initGDT() {
	gdt.setGDT((GDT::LameDescriptor*)_gdt); // TODO: strict aliasing violation

	// Null descriptor
	GDT::CoolDescriptor nulldesc;
	gdt.addDescriptor(nulldesc);

	// The order cannot be changed because of syscall

	// Kernel code
	GDT::CoolDescriptor kcode;
	kcode.setPresent();
	kcode.setUser();
	kcode.setCode();
	gdt.addDescriptor(kcode);

	// Kernel data
	GDT::CoolDescriptor kdata;
	kdata.setPresent();
	kdata.setUser();
	gdt.addDescriptor(kdata);

	// User data
	GDT::CoolDescriptor udata;
	udata.setPresent();
	udata.setUser();
	udata.setRing3();
	gdt.addDescriptor(udata);

	// User code
	GDT::CoolDescriptor ucode;
	ucode.setPresent();
	ucode.setUser();
	ucode.setCode();
	ucode.setRing3();
	gdt.addDescriptor(ucode);

	gdt.load();
}

uint16_t newTSSsegment(uint64_t addr) {
	GDT::CoolTSSDescriptor desc;
	desc.base = addr;
	gdt.addTSS(desc);
	gdt.load();
	return (gdt.getctr()-2)*GDT_LAME_DESCRIPTOR_SIZE;
}
