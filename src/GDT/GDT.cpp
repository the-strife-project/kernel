#include "GDT.hpp"
#include <klibc/klibc.hpp>

GDT::LameDescriptor GDT::CoolDescriptor::getLame() const {
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

GDT::LameTSSDescriptor GDT::CoolTSSDescriptor::getLame() const {
	LameTSSDescriptor lame;
	lame.base_lo16 = (uint16_t)(base & 0xFFFF);
	lame.base_mid8 = (uint8_t)((base >> 16) & 0xFF);
	lame.base_hi8 = (uint8_t)((base >> 24) & 0xFF);
	lame.base_rhi = (uint32_t)((base >> 32) & 0xFFFFFFFF);
	return lame;
}

void GDT::addDescriptor(GDT::CoolDescriptor cool) {
	gdt[ctr++] = cool.getLame();
}

void GDT::addTSS(GDT::CoolTSSDescriptor cool) {
	// TODO strict aliasing violation
	LameTSSDescriptor lame(cool.getLame());
	gdt[ctr++] = *((GDT::LameDescriptor*)&lame);
	gdt[ctr++] = *((GDT::LameDescriptor*)&lame + 1);
}

extern "C" void __loadGDT(uint64_t, uint64_t);
void GDT::load() {
	uint16_t limit = (ctr * sizeof(LameDescriptor)) - 1;
	__loadGDT(limit, (uint64_t)gdt);
}
