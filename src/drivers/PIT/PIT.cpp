#include "PIT.hpp"
#include "../APIC/APIC.hpp"
#include <CPU/SMP/SMP.hpp>
#include <klibc/klibc.hpp>
#include <IDT/MyIDT.hpp>

uint8_t PIT::gsi = ~0;

static void setFrequency(uint16_t ms) {
	uint16_t reloadValue = (ms * PIT::INPUT_CLOCK_FREQUENCY) / 3000;
	PIT::sendCommand(PIT::ICW);
	PIT::sendData(reloadValue & 0xFF);
	PIT::sendData(reloadValue >> 8);
}

extern "C" void PIT_ISR();

void PIT::init() {
	// Get GSI from IRQ
	bool gotGSI = false;
	for(auto const& x : APIC::IOAPIC::isos) {
		if(x.irq == PIT::IRQ) {
			PIT::gsi = x.gsi;
			gotGSI = true;
			break;
		}
	}

	if(!gotGSI)
		panic(Panic::NO_GSI_FOR_PIT);

	// Set the frequency
	setFrequency(1); // Tick each 1ms

	// Redirect IRQ to interupt vector using IOAPIC
	APIC::IOAPIC::setIRQRedirection(whoami(), VEC, gsi, false);

	// And set ISR in the IDT for the interrupt vector
	IDT::CoolDescriptor entry;
	entry.setPresent();
	entry.offset = (uint64_t)&PIT_ISR;
	entry.ist = 0; // Pick any
	IDTset(VEC, entry);
}

extern "C" void PIT_wait(size_t ms);
void PIT::sleep(size_t ms) {
	// This function uses this PIT to wait exactly 10ms, in order
	//   to calibrate the other clocks

	// Eyeballing it with a physical timer looks like it's halfed
	ms *= 2;

	// Disable interrupts
	cli();

	// Unmask GSI in the IOAPIC
	APIC::IOAPIC::unmask(gsi);

	// That's it. Go!
	PIT_wait(ms);

	// Mask again
	APIC::IOAPIC::mask(gsi);
}
