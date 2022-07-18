#ifndef PAGING_HPP
#define PAGING_HPP

#include <asm.hpp>
#include <types>

#define PAGE_ENTRIES 512

// Important point: Paging does shallow copy. You will see a lot of
//   "Paging paging" arguments and such. It's fine. You can understand
//   the object as an overcomplicated PML4E*, it's the only member.
class Paging {
public:
	#include "pagingStructs.hpp"
	#include "../map/map.hpp"
	friend class PageMapping;

	struct MapFlag {
		static const uint64_t GLOBAL = 1 << 0;
		static const uint64_t RO = 1 << 1;
		static const uint64_t NX = 1 << 2;
		static const uint64_t USER = 1 << 3;
		static const uint64_t PCD = 1 << 4; // Page Cache Disable
	};

private:
	PML4E* data = nullptr;

	// TODO: Make page table RO until necessary for safety

public:
	inline void setData(PML4E* x) { data = x; }
	inline PML4E* getData() { return data; }
	static void getIndexes(uint64_t addr, uint64_t& pml4_i, uint64_t& pdp_i, uint64_t& pd_i, uint64_t& pt_i);

	PTE* getPTE(uint64_t addr);
	uint64_t getPhys(uint64_t virt);
	void map(uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags=0);
	void unmap(uint64_t virt);
	uint64_t getFlags(uint64_t virt);
	inline void load() { setCR3((uint64_t)data); }
	void dump();
	void destroy();

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
