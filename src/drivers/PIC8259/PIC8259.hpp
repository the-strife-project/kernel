#ifndef PIC8259_HPP
#define PIC8259_HPP

// TODO: I've just realized this doesn't have to be in the kernel :^)

#include <asm.hpp>

#define PIC1_IO   0x20
#define PIC1_CMD  PIC1_IO
#define PIC1_DATA (PIC1_IO+1)

#define PIC2_IO   0xA0
#define PIC2_CMD  PIC2_IO
#define PIC2_DATA (PIC2_IO+1)

#define PIC_IRQ0 PIC1_IO
#define PIC_EOI 0x20

namespace PIC {
	void setMask();
	inline void refresh() {
		setMask();
		sti();
	}

	void init();
	void EOI();
	void up(uint8_t irq);
	void down(uint8_t irq);
};

#endif
