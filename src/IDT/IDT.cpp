#include "IDT.hpp"
#include <klibc/klibc.hpp>

IDT::LameDescriptor IDT::CoolDescriptor::getLame() const {
	LameDescriptor ret;
	ret.offset_lo16 = offset & 0xFFFF;
	ret.offset_mid16 = (offset >> 16) & 0xFFFF;
	ret.offset_hi32 = (offset >> 32) & 0xFFFFFFFF;
	ret.ist = ist;
	ret.attributes = attributes;
	return ret;
}

void IDT::load() {
	idtr.offset = (uint64_t)idt;
	idtr.size = (sizeof(LameDescriptor) * N_ENTRIES_IDT) - 1;
	asm volatile("lidt (%0)" : : "r"(&idtr));
}
