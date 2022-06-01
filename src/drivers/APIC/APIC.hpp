#ifndef APIC_HPP
#define APIC_HPP

#include <asm.hpp>

namespace APIC {
	const MSR BASE_MSR = 0x1B;
	const size_t LAPIC_ENABLE = 1 << 11;

	uint32_t* const ADDRESS = (uint32_t*)0xFEE00000;

	const size_t EOI_OFFSET = 0xB0;
	const size_t SIVR_OFFSET = 0xF0;

	bool isSupported();

	inline uint32_t readReg(size_t offset) {
		return ADDRESS[offset / sizeof(uint32_t)];
	}

	inline void writeReg(size_t offset, uint32_t val) {
		ADDRESS[offset / sizeof(uint32_t)] = val;
	}

	void init();

	inline void EOI() { writeReg(EOI_OFFSET, 0); }
};

#endif
