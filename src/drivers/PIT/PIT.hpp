#ifndef PIT_HPP
#define PIT_HPP

// This directory implements a PIT driver so that the APIC timer driver
//   has a reference to measure the frequecy.

#include <types>
#include <driver>

namespace PIT {
	const uint8_t IRQ = 0;
	extern uint8_t gsi;
	const uint8_t VEC = 32; // Interrupt vector chosen

	const uint16_t CHANNEL0_DATA_PORT = 0x40;
	const uint16_t CMD_PORT  = 0x43;
	const uint16_t ICW = 0x36; // Channel 0, lobyte/hibyte, rate generator, binary
	const uint32_t INPUT_CLOCK_FREQUENCY = 3579545;

	inline void sendCommand(uint8_t cmd) { std::out8(CMD_PORT, cmd); }
	inline void sendData(uint8_t data) { std::out8(CHANNEL0_DATA_PORT, data); }

	void init();
	void sleep(size_t ms);
};

#endif
