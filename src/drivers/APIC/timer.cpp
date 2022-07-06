#include "APIC.hpp"
#include "../PIT/PIT.hpp"
#include <IDT/MyIDT.hpp>
#include <CPU/TSS/ISTs.hpp>

extern "C" void asmPreemption();
uint32_t APIC::ticks10ms = 0;

// Calibrates the APIC timer
void APIC::initTimer() {
	// Initialize PIT so it's possible to sleep later
	PIT::init();

	// Use divider 16 in APIC timer
	writeReg(APIC::TIMER_DIVIDER, 0x3);

	// Set APIC init counter to -1
	writeReg(APIC::TIMER_INITIAL_COUNT, 0xFFFFFFFF);

	// Sleep 10 ms
	PIT::sleep(10);

	// Get count
	ticks10ms = readReg(TIMER_CURRENT_COUNT);
	ticks10ms = ~0 - ticks10ms;

	// Now, set the new ISR
	IDT::CoolDescriptor entry;
	entry.setPresent();
	entry.offset = (uint64_t)&asmPreemption;
	entry.ist = IST_APIC_TIMER;
	IDTset(PIT::VEC, entry); // IRQ 0 again

	// That's it. When dispatching, a one-shot mode will be set
}

void APIC::startQuantum() {
	// Start one-shot timer on IRQ 0, divider 16
	writeReg(TIMER_LVT, 32 | MODE_ONESHOT);
	writeReg(TIMER_DIVIDER, 0x3);
	writeReg(TIMER_INITIAL_COUNT, ticks10ms);
}
