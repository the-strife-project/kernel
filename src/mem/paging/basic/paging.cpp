#include "paging.hpp"
#include <mem/PMM/PMM.hpp>
#include <panic/panic.hpp>
#include <klibc/klibc.hpp>

inline uint64_t getPML4i(uint64_t addr) { return (addr >> (9*3+12)) & 0x1FF; }
inline uint64_t getPDPi(uint64_t addr)  { return (addr >> (9*2+12)) & 0x1FF; }
inline uint64_t getPDi(uint64_t addr)   { return (addr >> (9*1+12)) & 0x1FF; }
inline uint64_t getPTi(uint64_t addr)   { return (addr >> (9*0+12)) & 0x1FF; }

void Paging::getIndexes(uint64_t addr, uint64_t& pml4_i, uint64_t& pdp_i, uint64_t& pd_i, uint64_t& pt_i) {
	pml4_i = getPML4i(addr); pdp_i = getPDPi(addr);
	pd_i = getPDi(addr); pt_i = getPTi(addr);
}

Paging::PTE* Paging::getPTE(uint64_t addr) {
	uint64_t pml4_i, pdp_i, pd_i, pt_i;
	getIndexes(addr, pml4_i, pdp_i, pd_i, pt_i);

	PML4E* pml4e = data;
	pml4e = &pml4e[pml4_i];

	if(!pml4e->isPresent())
		panic(Panic::GET_PTE_NO_PAGE);

	PDPE* pdpe = (PDPE*)extend(pml4e->getNext() << 12);
	pdpe = &pdpe[pdp_i];

	if(!pdpe->isPresent())
		panic(Panic::GET_PTE_NO_PAGE);

	PDE* pde = (PDE*)extend(pdpe->getNext() << 12);
	pde = &pde[pd_i];

	if(!pde->isPresent())
		panic(Panic::GET_PTE_NO_PAGE);
	else if(pde->isHuge())
		extendPDE(pde);

	PTE* pte = (PTE*)extend(pde->getNext() << 12);
	pte = &pte[pt_i];

	if(!pte->isPresent())
		panic(Panic::GET_PTE_NO_PAGE);

	return pte;
}

// Extends a huge PDE into a regular one
void Paging::extendPDE(PDE* pde) {
	uint64_t phys = pde->getNext();
	PTE* pageTable = (PTE*)PMM::calloc();
	pde->setNext(((uint64_t)pageTable) >> 12);

	for(size_t i=0; i<PAGE_ENTRIES; ++i) {
		pageTable->setPhys(phys);
		if(pde->isGlobal()) pageTable->setGlobal();
		if(pde->isUser()) pageTable->setUser();
		if(pde->isRO()) pageTable->setRO();
		if(pde->isNX()) pageTable->setNX();
		++pageTable; ++phys;
	}
}

// Wrapper around PageMapping
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
