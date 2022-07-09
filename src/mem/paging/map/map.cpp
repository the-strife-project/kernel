#include <mem/paging/basic/paging.hpp>
#include <mem/PMM/PMM.hpp>
#include <klibc/klibc.hpp>
#include <panic/panic.hpp>

#define PAGE_ENTRIES 512

// TODO possibly many strict aliasing violations

Paging::PageMapping::PageMapping(Paging paging, uint64_t virt)
	: data(paging.data), virt(virt)
{
	getIndexes(virt, pml4_i, pdp_i, pd_i, pt_i);
}

void Paging::PageMapping::nextPML4E() {
	pml4e = &data[pml4_i++];
	if(!pml4e->isPresent()) {
		*pml4e = PML4E();
		pml4e->setPresent();
		pml4e->setNext(clear(PhysMM::calloc() >> 12));
	}

	if(user)
		pml4e->setUser();
}

void Paging::PageMapping::nextPDPE() {
	if(pdp_i >= PAGE_ENTRIES) {
		pdp_i = 0;
		nextPML4E();
	} else if(!pml4e) nextPML4E();

	pdpe = (PDPE*)(extend(pml4e->getNext() << 12));
	pdpe = &pdpe[pdp_i++];

	if(!pdpe->isPresent()) {
		*pdpe = PDPE();
		pdpe->setPresent();
		pdpe->setNext(clear(PhysMM::calloc() >> 12));
	}

	if(user) pdpe->setUser();
}

void Paging::PageMapping::nextPDE(bool huge) {
	if(pd_i >= PAGE_ENTRIES) {
		pd_i = 0;
		nextPDPE();
	} else if(!pdpe) nextPDPE();

	pde = (PDE*)(extend(pdpe->getNext() << 12));
	pde = &pde[pd_i++];

	if(!pde->isPresent()) {
		*pde = PDE();
		pde->setPresent();

		if(!huge)
			pde->setNext(clear(PhysMM::calloc() >> 12));
	}

	if(user) pde->setUser();
	if(huge) pde->setHuge();
}

void Paging::PageMapping::nextPTE() {
	if(pt_i >= PAGE_ENTRIES) {
		pt_i = 0;
		nextPDE(false);
	} else if(!pde) nextPDE(false);

	pte = (PTE*)(extend(pde->getNext() << 12));
	pte = &pte[pt_i++];
	*pte = PTE();
}

void Paging::PageMapping::map4K(uint64_t phys) {
	nextPTE();
	if(present) pte->setPresent();
	pte->setPhys(phys >> 12);

	if(global) pte->setGlobal();
	if(user) pte->setUser();
	if(ro) pte->setRO();
	if(nx) pte->setNX();
	if(pcd) pte->setPCD();
}

void Paging::PageMapping::map2M(uint64_t phys) {
	nextPDE(true);
	pde->setNext(phys >> 12);

	if(global) pde->setGlobal();
	if(user) pde->setUser();
	if(ro) pde->setRO();
	if(nx) pde->setNX();
}
