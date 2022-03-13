#ifndef GDT_HPP
#define GDT_HPP

#include <common.hpp>

#define GDT_ACCESS_RW 1
#define GDT_ACCESS_CODE 3
#define GDT_ACCESS_SYSTEM 4
#define GDT_ACCESS_DPL 5
#define GDT_ACCESS_PRESENT 7

#define GDT_FLAG_LONG 1
#define GDT_FLAG_GRANULARITY 3

#define GDT_TSS_TYPE 0x09

#define GDT_LAME_DESCRIPTOR_SIZE 0x08

/*
	Note that this GDT implementation is not generic, it's mainly
	based on the fact that this is for long mode.
*/

class GDT {
public:
	struct LameDescriptor {
		uint16_t limit_lo16;
		uint16_t base_lo16;
		uint8_t base_mid8;
		uint16_t flags;
		uint8_t base_hi8;
	} __attribute__((packed));

	struct LameTSSDescriptor {
		uint16_t limit_lo16 = 104;	// TSS_SIZE
		uint16_t base_lo16;
		uint8_t base_mid8;
		uint16_t flags = GDT_TSS_TYPE | (1 << GDT_ACCESS_PRESENT) | (3 << GDT_ACCESS_DPL);
		uint8_t base_hi8;
		uint32_t base_rhi;	// Really high lmao
		uint32_t zero = 0;
	} __attribute__((packed));

private:
	LameDescriptor* gdt;
	size_t ctr = 0;

public:
	class CoolDescriptor {
		/* Default type is data. */
	private:
		uint8_t flags = (1 << GDT_FLAG_GRANULARITY);
		uint8_t access = (1 << GDT_ACCESS_RW);

	public:
		uint32_t base = 0;
		uint32_t limit = ~0;

		inline void setTSS() { flags = GDT_TSS_TYPE; }
		inline void setPresent() { access |= 1 << GDT_ACCESS_PRESENT; }
		inline void setUser() { access |= 1 << GDT_ACCESS_SYSTEM; }
		inline void setRing3() { access |= 3 << GDT_ACCESS_DPL; }

		inline void setCode() {
			access |= (1 << GDT_ACCESS_CODE);
			flags |= (1 << GDT_FLAG_LONG);
		}

		LameDescriptor getLame() const;
	};

	class CoolTSSDescriptor {
	public:
		uint64_t base;
		LameTSSDescriptor getLame() const;
	};

	inline void setGDT(LameDescriptor* x) { gdt = x; }
	void addDescriptor(CoolDescriptor);
	void addTSS(CoolTSSDescriptor);
	void load();
	inline size_t getctr() const { return ctr; }
};


void initGDT();

#endif
