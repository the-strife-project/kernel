#ifndef PAGING_HPP
#define PAGING_HPP

#include <asm.hpp>

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 512

class Paging {
public:
	#include "pagingStructs.hpp"
	#include "../map/map.hpp"
	friend class PageMapping;

	struct MapFlag {
		static const uint64_t GLOBAL = 1 << 0;
		static const uint64_t RO = 1 << 1;
		static const uint64_t NX = 1 << 2;
	};

private:
	PML4E* data;

public:
	inline void setData(PML4E* x) { data = x; }
	static void getIndexes(uint64_t addr, uint64_t& pml4_i, uint64_t& pdp_i, uint64_t& pd_i, uint64_t& pt_i);

	PTE* getPTE(uint64_t addr);
	static void extendPDE(PDE*);
	void map(uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags=0);
	inline void reload() { setCR3((uint64_t)data); }

	// Sign stuff (36 bits)
	static const uint64_t shift = 0x1000000000;
	static inline uint64_t clear(uint64_t x) { return x & (shift-1); }
	static inline uint64_t extend(uint64_t x) {
		if(x & shift)
			return x | ~(shift-1);
		else
			return clear(x);
	}
};

#endif
