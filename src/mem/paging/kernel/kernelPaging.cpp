#include "kernelPaging.hpp"
#include <mem/PMM/PMM.hpp>
#include <klibc/klibc.hpp>

#define CR4_PAGE_GLOBAL 7

#define EFER_ADDR 0xC0000080
#define EFER_BIT_NX 11

Paging kpaging;
uint64_t kernelBegin, kernelEnd;

bool isPagingAvailable = false;

void initKernelPaging(MemoryMap mm) {
	// Enable global-page extensions
	setCR4(getCR4() | (1 << CR4_PAGE_GLOBAL));

	// Enable NX on the EFER
	uint64_t efer = rdmsr(EFER_ADDR);
	efer |= 1 << EFER_BIT_NX;
	wrmsr(EFER_ADDR, efer);

	// Initialize the kpaging object
	kpaging.setData((Paging::PML4E*)PhysMM::calloc());

	// Map the framebuffer
	kpaging.map(0xFFFFFFFF800B8000, 0xB8000, 80*25*2, Paging::MapFlag::NX);

	// Map the kernel and the modules
	for(auto const& x : mm) {
		if(x.type == STIVALE2_MMAP_USABLE || x.type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE) {
			kpaging.map(x.base, x.base, x.length, Paging::MapFlag::NX);
		} else if(x.type == STIVALE2_MMAP_KERNEL_AND_MODULES) {
			uint64_t base = PAGE(x.base);
			uint64_t baseoff = x.base - base;
			kpaging.map(HIGHER_HALF + base, base, x.length + baseoff, Paging::MapFlag::GLOBAL);
		}
	}

	kpaging.load();
	isPagingAvailable = true;
}
