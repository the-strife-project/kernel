#include "PMM.hpp"
#include <bitmap>
#include <klibc/spinlock.hpp>
#include <klibc/memory/memory.hpp>
#include <mem/paging/paging.hpp>

static size_t usedPages = 0;

static size_t allocInRegion(PhysMM::Frame* frame, size_t want) {
	uint8_t* start = (uint8_t*)frame + sizeof(PhysMM::Frame);
	uint64_t pages = frame->pages;

	size_t have = 0;

	std::bitmap bm(pages, start);
	size_t cur = 0;//frame->firstFree;
	size_t ret = cur;
	while(cur < pages) {
		if(!bm.get(cur)) {
			++have;
		} else {
			ret = cur+1;
			have = 0;
		}

		if(have == want) {
			// Nice. Set the bits.
			for(size_t i=0; i<want; ++i)
				bm.set(ret+i, true);
			usedPages += want;
			return ret;
		}

		++cur;
	}

	// Tough luck
	return pages; // Impossible value
}

static Spinlock lock;

uint64_t PhysMM::alloc(size_t npages) {
	uint64_t ret = 0;
	lock.acquire();

	for(size_t i=0; i<nregions; ++i) {
		PhysMM::Frame* frame = regions[i];
		size_t got = allocInRegion(frame, npages);
		if(got != frame->pages) {
			// Valid index!
			ret = frame->first + got * PAGE_SIZE;

			// Set used chunks to 0
			if(isPagingAvailable)
				for(size_t j=0; j<npages; ++j)
					kpaging.getPTE(ret + j * PAGE_SIZE)->setUsedChunks(0);

			break;
		}
	}

	lock.release();
	return ret;
}

static void freeFromFrame(uint64_t phys, size_t npages, PhysMM::Frame* frame) {
	uint8_t* start = (uint8_t*)frame + sizeof(PhysMM::Frame);
	uint64_t pages = frame->pages;

	size_t begin = (phys - frame->first) / PAGE_SIZE;

	std::bitmap bm(pages, start);
	for(size_t i=0; i<npages; ++i)
		bm.set(begin+i, false);
	usedPages -= npages;
}

void PhysMM::free(uint64_t phys, size_t npages) {
	lock.acquire();
	// npages cannot pass region boundary

	// Which region is phys in?
	Frame* best = nullptr;
	for(size_t i=0; i<nregions; ++i) {
		if((uint64_t)(regions[i]) < phys && regions[i] > best)
			best = regions[i];
	}

	if(!best)
		bruh(Bruh::PMM_NO_REGION);

	freeFromFrame(phys, npages, best);
	lock.release();
}

uint64_t PhysMM::calloc(size_t npages) {
	auto ret = alloc(npages);
	memset((void*)ret, 0, PAGE_SIZE);
	return ret;
}

size_t PhysMM::getUsedPages() { return usedPages; }
