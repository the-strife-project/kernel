#ifndef APIC_HPP
#define APIC_HPP

#include <asm.hpp>
#include <klibc/list.hpp>

namespace APIC {
	const MSR BASE_MSR = 0x1B;
	const size_t LAPIC_ENABLE = 1 << 11;

	uint32_t* const ADDRESS = (uint32_t*)0xFEE00000;

	const size_t EOI_OFFSET = 0xB0;
	const size_t SIVR_OFFSET = 0xF0;

	const size_t TIMER_DIVIDER = 0x3E0;
	const size_t TIMER_INITIAL_COUNT = 0x380;
	const size_t TIMER_CURRENT_COUNT = 0x390;
	const size_t TIMER_LVT = 0x320;
	const size_t TIMER_MASK = 1 << 16;
	const size_t MODE_PERIODIC = 1 << 17;
	const size_t MODE_ONESHOT = 0;

	bool isSupported();

	inline uint32_t readReg(size_t offset) {
		return ADDRESS[offset / sizeof(uint32_t)];
	}

	inline void writeReg(size_t offset, uint32_t val) {
		ADDRESS[offset / sizeof(uint32_t)] = val;
	}

	void init();

	inline void EOI() { writeReg(EOI_OFFSET, 0); }

	// --- IOAPIC ---
	namespace IOAPIC {
		typedef size_t GSI;

		// Register offsets
		const size_t IOREGSEL = 0;
		const size_t IOWIN = 0x10;

		// Redirecion entry constants for IRQs
		const size_t FLAG_EDGE_TRIGGERED = 1 << 1;
		const size_t FLAG_LEVEL_TRIGGERED = 1 << 3;
		// ---
		const uint64_t INTPOL = 1 << 13;
		const uint64_t TRIGGER_MODE = 1 << 15;
		const uint64_t MASKED = 1 << 16;
		const uint64_t LID_SHIFT = 56;

		// Runtime information
		extern uint8_t id;
		extern uint32_t address;
		extern uint32_t gsiBase;

		// Structures
		struct MADT_IOAPIC {
			uint8_t id;
			uint8_t reserved;
			uint32_t address;
			uint32_t gsiBase;
		} __attribute__((packed));
		// Interrupt Source Override
		struct MADT_ISO {
			uint8_t bus;
			uint8_t irq;
			uint32_t gsi;
			uint16_t flags;
		} __attribute__((packed));

		// ISOs
		extern List<MADT_ISO> isos;

		// Procedures
		void init();
		uint32_t readReg(uint8_t field);
		void writeReg(uint8_t field, uint32_t val);
		size_t readRedirection(GSI gsi);
		void writeRedirection(GSI gsi, uint64_t entry);
		void setIRQRedirection(uint32_t lapicID, uint8_t vec, GSI gsi, size_t flags);
		void mask(GSI gsi);
		void unmask(GSI gsi);
	}

	// --- TIMER ---
	void initTimer();
	extern "C" uint32_t ticks10ms;
	void startQuantum();
	void anotherChance();
};

#endif
