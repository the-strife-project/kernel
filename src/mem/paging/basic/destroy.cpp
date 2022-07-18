#include "paging.hpp"
#include <mem/PMM/PMM.hpp>

void Paging::destroy() {
	// Look for all allocated PTEs and free them
	// Free PTs, PDs, PDPs, and the PML4 in the process too

	PML4E* pml4es = data;
	for(size_t i=0; i<PAGE_ENTRIES; ++i) {
		PML4E* pml4e = &pml4es[i];
		if(!pml4e->isPresent()) continue;
		if(!pml4e->isGlobal()) continue;

		PDPE* pdpes = (PDPE*)(pml4e->getNext() << 12);
		for(size_t j=0; j<PAGE_ENTRIES; ++j) {
			PDPE* pdpe = &pdpes[j];
			if(!pdpe->isPresent()) continue;
			if(!pdpe->isGlobal()) continue;

			PDE* pdes = (PDE*)(pdpe->getNext() << 12);
			for(size_t k=0; k<PAGE_ENTRIES; ++k) {
				PDE* pde = &pdes[k];
				if(!pde->isPresent()) continue;
				if(!pde->isGlobal()) continue;

				PTE* ptes = (PTE*)(pde->getNext() << 12);
				for(size_t l=0; l<PAGE_ENTRIES; ++l) {
					PTE* pte = &ptes[l];
					if(!pte->isPresent()) continue;
					if(!pte->isGlobal()) continue;

					// Go read mmap.cpp's munmap
					if(pte->getUsedChunks() < 2) {
						PhysMM::freeOne(pte->getPhys() << 12);
					} else {
						pte->decUsedChunks();
					}
				}

				PhysMM::freeOne((uint64_t)ptes);
			}

			PhysMM::freeOne((uint64_t)pdes);
		}

		PhysMM::freeOne((uint64_t)pdpes);
	}

	PhysMM::freeOne((uint64_t)data);
	data = nullptr;
}
