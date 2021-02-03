#include "PIC8259.hpp"

static uint16_t irqmask = 0xFFFF & ~(1 << 2);

void PIC::setMask() {
	outb(PIC1_DATA, irqmask);
	outb(PIC2_DATA, irqmask >> 8);
}

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

void PIC::init() {
	// ICW1: begin initialization
	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

	// ICW2: remap offset address of IDT
	outb(PIC1_DATA, PIC_IRQ0);
	outb(PIC2_DATA, PIC_IRQ0 + 8);

	// ICW3: setup cascading
	outb(PIC1_DATA, 4);
	outb(PIC2_DATA, 2);

	// ICW4: environment info
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

	PIC::setMask();
}

void PIC::EOI() {
	outb(PIC1_CMD, PIC_EOI);
	outb(PIC2_CMD, PIC_EOI);
}

void PIC::up(uint8_t irq) {
	irqmask &= ~(1 << irq);
	PIC::refresh();
}

void PIC::down(uint8_t irq) {
	irqmask |= 1 << irq;
	PIC::refresh();
}
