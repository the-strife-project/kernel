#include "GDT.hpp"

GDT::LameDescriptor GDT::CoolDescriptor::getLame() {
	LameDescriptor lame;
	lame.limit_lo16 = (uint16_t)(limit & 0xFFFF);
	lame.base_lo16 = (uint16_t)(base & 0xFFFF);
	lame.base_mid8 = (uint8_t)((base >> 16) & 0xFF);
	lame.flags = access;
	lame.flags |= (uint16_t)((limit >> 16) & 0x0F) << 8;
	lame.flags |= (uint16_t)flags << 12;
	lame.base_hi8 = (uint8_t)((base >> 24) & 0xFF);
	return lame;
}

void GDT::addDescriptor(GDT::CoolDescriptor cool) {
	gdt[ctr++] = cool.getLame();
}

void GDT::load() {
	gdtptr.base = (uint64_t)gdt;
	gdtptr.limit = (ctr * sizeof(LameDescriptor)) - 1;
	asm volatile ("lgdt (%0)" : : "r"(&gdtptr));
}
