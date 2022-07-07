#ifndef TSS_HPP
#define TSS_HPP

#include <GDT/MyGDT.hpp>

#define TSS_SIZE 104

#include "ISTs.hpp"

class TSS {
private:
	class _TSS {
	private:
		uint32_t reserved = 0;
		uint64_t RSPs[3] = {0};
		uint64_t reserved2 = 0;
		uint64_t ISTs[7] = {0};
		uint64_t reserved3 = 0;
		uint16_t reserved4 = 0;
		uint16_t ipob = TSS_SIZE;

	public:
		inline void setRSP0(uint64_t rsp0) { RSPs[0] = rsp0; }
		inline void setRSP1(uint64_t rsp1) { RSPs[1] = rsp1; }
		inline void setIST(size_t n, uint64_t rsp) { ISTs[n - 1] = rsp; }
	} __attribute__((packed));

	_TSS* tss;
	uint16_t desc;

public:
	inline uint64_t getAddr() { return (uint64_t)tss; }
	inline void create() {
		tss = (_TSS*)alloc(TSS_SIZE, PUBLIC);
		*tss = _TSS();
		desc = newTSSsegment((uint64_t)tss);
	}
	inline void setRSP0(uint64_t rsp0) { tss->setRSP0(rsp0); }
	inline void setRSP1(uint64_t rsp1) { tss->setRSP1(rsp1); }
	inline void setIST(size_t n, uint64_t rsp) { tss->setIST(n, rsp); }
	inline void load() { asm volatile("ltr %%ax" :: "a"(desc) : "cc"); }
};

inline TSS newTSS() {
	TSS ret;
	ret.create();
	return ret;
}

#endif
