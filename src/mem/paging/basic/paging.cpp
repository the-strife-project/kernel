#include "paging.hpp"
#include <mem/PMM/PMM.hpp>

inline uint64_t getPML4i(uint64_t addr) { return (addr >> (9*3+12)) & 0x1FF; }
inline uint64_t getPDPi(uint64_t addr)  { return (addr >> (9*2+12)) & 0x1FF; }
inline uint64_t getPDi(uint64_t addr)   { return (addr >> (9*1+12)) & 0x1FF; }
inline uint64_t getPTi(uint64_t addr)   { return (addr >> (9*0+12)) & 0x1FF; }

void Paging::getIndexes(uint64_t addr, uint64_t& pml4_i, uint64_t& pdp_i, uint64_t& pd_i, uint64_t& pt_i) {
	pml4_i = getPML4i(addr); pdp_i = getPDPi(addr);
	pd_i = getPDi(addr); pt_i = getPTi(addr);
}

#define TWOMEGS 2 << 20
#define FOURKBS 4 << 10

void Paging::map(uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags) {
	PageMapping mapping(*this, virt);

	if(flags & MapFlag::GLOBAL)
		mapping.setGlobal();
	if(flags & MapFlag::RO)
		mapping.setRO();
	if(flags & MapFlag::NX)
		mapping.setNX();

	if(size % PAGE_SIZE)
		size += PAGE_SIZE - (size % PAGE_SIZE);

	while(size) {
		// Can we map 2M?
		if(size >= TWOMEGS && getPTi(virt) == 0) {
			mapping.map2M(phys);
			virt += TWOMEGS; phys += TWOMEGS; size -= TWOMEGS;
			continue;
		}

		// Just map 4K
		mapping.map4K(phys);
		virt += FOURKBS; phys += FOURKBS; size -= FOURKBS;
	}
}
