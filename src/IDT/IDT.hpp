#ifndef IDT_HPP
#define IDT_HPP

#include <common.hpp>
#include <GDT/MyGDT.hpp>
#include <klibc/klibc.hpp>
#include "enum.hpp"

#define N_ENTRIES_IDT 256

/*
	Same as GDT, this is not a generic implementation.
*/

#define IDT_ATTR_GATE 0
#define IDT_ATTR_DPL 5
#define IDT_ATTR_PRESENT 7

#define IDT_GATE_INTERRUPT 0b1110

class IDT {
public:
	struct IDTR {
		uint16_t size;
		uint64_t offset;
	} __attribute__((packed));

	struct LameDescriptor {
		uint16_t offset_lo16;
		uint16_t cs = SEGMENT_SELECTOR_KCODE;
		uint8_t ist = 0;
		uint8_t attributes;
		uint16_t offset_mid16;
		uint32_t offset_hi32;
		uint32_t zero = 0;
	} __attribute__((packed));

private:
	LameDescriptor* idt;
	IDTR idtr;

public:
	struct FakeDescriptor { uint64_t _[2]; };

	class CoolDescriptor {
	private:
		uint8_t attributes = (IDT_GATE_INTERRUPT << IDT_ATTR_GATE);
		bool ring3 = false;

	public:
		uint64_t offset;
		size_t ist = 0;

		inline void setRing3() { attributes |= 3 << IDT_ATTR_DPL; ring3 = true; }
		inline void setPresent() { attributes |= 1 << IDT_ATTR_PRESENT; }

		LameDescriptor getLame() const;
	};

	// TODO strict aliasing violation
	inline void setIDT(FakeDescriptor* x) { idt = (LameDescriptor*)x; }
	inline void set(uint8_t n, const CoolDescriptor& cool) { idt[n] = cool.getLame(); }
	void load();
};

#endif
