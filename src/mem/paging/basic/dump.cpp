#include "paging.hpp"
#include <klibc/output/output.hpp>

/*
	BigPaging takes advantage of the fact that the PML4, PDP, and PDE structures
	are the same, just so the code is less repetitive.
*/

typedef Paging::PML4E BigPaging; // PML4E = PDPE = PDE

void bigDump(BigPaging*, size_t base, size_t ctr);
void ptDump(Paging::PDE*, size_t base);

void Paging::dump() {
	printf("\n--- PAGE TABLE DUMP ---\n");
	bigDump((Paging::PML4E*)(this->data), 0, 2);
}

void bigDump(BigPaging* big, size_t base, size_t ctr) {
	for(size_t i=0; i<PAGE_ENTRIES; ++i) {
		BigPaging* cur = &big[i];
		if(!cur->isPresent()) continue;

		size_t mult = PAGE_SIZE;
		for(size_t j=0; j<ctr; ++j)
			mult *= PAGE_ENTRIES;
		size_t begin = base + i * mult;
		size_t end = base + (i+1) * mult;
		for(size_t j=0; j<2-ctr; ++j)
			printf("  ");
		printf("0x%x-0x%x:", begin, end);

		if(cur->isRO()) printf(" RO");
		if(cur->isUser()) printf(" USER");
		if(cur->isNX()) printf(" NX");
		if(cur->isHuge()) printf(" HUGE");
		if(cur->isGlobal()) printf(" GLOBAL");

		printf("\n");

		// TODO strict aliasing violation?
		BigPaging* next = (BigPaging*)Paging::extend(cur->getNext() << 12);
		if(ctr)
			bigDump(next, begin, ctr-1);
		else
			ptDump((Paging::PDE*)next, base);
		// ↑ TODO strict aliasing violation
	}
}

void ptDump(Paging::PDE* pde, size_t base) {
	for(size_t i=0; i<PAGE_ENTRIES; ++i) {
		// TODO strict aliasing violation
		Paging::PTE* pte = (Paging::PTE*)&(pde[i]);
		if(!pte->isPresent()) continue;

		size_t begin = base + i * PAGE_SIZE;
		size_t end = base + (i+1) * PAGE_SIZE;
		printf("      0x%x-0x%x:", begin, end);

		if(pte->isRO()) printf(" RO");
		if(pte->isUser()) printf(" USER");
		if(pte->isNX()) printf(" NX");
		if(pte->isGlobal()) printf(" GLOBAL");

		printf("\n");
	}
}
