#include "MyIDT.hpp"
#include <klibc/klibc.hpp>
#include <asm.hpp>
#include "exceptions/exceptions.hpp"
#include <CPU/TSS/ISTs.hpp>
#include "enum.hpp"

static IDT::FakeDescriptor _idt[N_ENTRIES_IDT];
static IDT idt;

extern uint64_t defaultISRs;

void initIDT() {
	idt.setIDT(_idt);
	uint64_t* isrs = &defaultISRs;

	for(size_t i=0; i<N_ENTRIES_IDT; ++i) {
		IDT::CoolDescriptor cool;
		cool.offset = isrs[i];
		cool.setPresent();
		idt.set(i, cool);
	}

	// Specific cases
	IDT::CoolDescriptor df;
	df.setPresent();
	df.offset = (uint64_t)&asmDF;
	df.ist = IST_DOUBLE_FAULT;
	idt.set(IDTException::DOUBLE_FAULT, df);

	IDT::CoolDescriptor pf;
	pf.setPresent();
	pf.offset = (uint64_t)&asmPF;
	pf.ist = IST_PAGE_FAULT;
	idt.set(IDTException::PAGE_FAULT, pf);

	idt.load();
}

static Spinlock lock;
void IDTsetIST(size_t i, uint8_t ist) {
	lock.acquire();
	((IDT::LameDescriptor*)_idt)[i].ist = ist;
	lock.release();
}

void IDTset(size_t i, const IDT::CoolDescriptor& desc) {
	lock.acquire();
	idt.set(i, desc);
	lock.release();
}
