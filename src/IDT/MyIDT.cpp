#include "IDT.hpp"
#include "MyIDT.hpp"
#include <klibc/klibc.hpp>
#include <asm.hpp>

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

	idt.load();
}

// Changing an entry will require a spinlock
