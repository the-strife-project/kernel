#include "APIC.hpp"
#include <klibc/klibc.hpp>

bool APIC::isSupported() {
	// CPUID.01h:EDX[9] specifies whether CPU has built-in local APIC

	size_t val;
	asm volatile("cpuid"
				 : "=d" (val)
				 : "a" (0x01));

	val &= 1 << 9;
	return val;
}

static void disablePIC() {
	outb(0xA1, 0xFF);
	outb(0x21, 0xFF);
}

static const size_t mapflags = Paging::MapFlag::NX | Paging::MapFlag::PCD;
static bool mapped = false;
static void enableLAPIC() {
	uint64_t base = rdmsr(APIC::BASE_MSR);
	base |= APIC::LAPIC_ENABLE;

	// Get APIC base address and map it to virtual memory
	uint64_t address = PAGE(base);
	if(address != (uint64_t)APIC::ADDRESS)
		panic(Panic::APIC_WEIRD_ADDRESS);
	if(!mapped && kpaging.getPhys(address))
		panic(Panic::APIC_ADDRESS_IN_USE);
	kpaging.map(address, address, PAGE_SIZE, mapflags);
	mapped = true;

	// Make sure it's on
	wrmsr(APIC::BASE_MSR, base);

	// Set SIVR bit 8 to start receiving interrupts
	uint32_t sivr = APIC::readReg(APIC::SIVR_OFFSET);
	sivr |= 1 << 8;
	APIC::writeReg(APIC::SIVR_OFFSET, sivr);
}

static bool disabledPIC = false;
void APIC::init() {
	if(!APIC::isSupported())
		panic(Panic::NO_APIC);

	if(!disabledPIC) {
		disablePIC();
		disabledPIC = true;
	}

	enableLAPIC();
	APIC::IOAPIC::init();

	// TODO: When SMP, check BSP, add lock around disabledPIC
}
