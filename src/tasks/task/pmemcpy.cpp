#include "task.hpp"
#include <algorithm>
#include <tasks/PIDs/PIDs.hpp>

static void* remoteToKernel(uint64_t ptr, Paging remote) {
	uint64_t ret = remote.getPhys(PAGE(ptr));
	if(!ret)
		return nullptr;
	ret += PAGEOFF(ptr);
	return (void*)ret;
}

// P(rocess) memcpy
// Copies buffers between address spaces
bool pmemcpy(void* dst, Paging remote, void* orig, size_t n) {
	size_t npages = (((size_t)orig) + n) / PAGE_SIZE;
	npages -= ((size_t)orig) / PAGE_SIZE;

	char* cdst = (char*)dst;
	char* corig = (char*)orig;

	// First page
	size_t sz = std::min(n, PAGE_SIZE - PAGEOFF((size_t)orig));
	void* remotePage = remoteToKernel((uint64_t)corig, remote);
	if(!remotePage)
		return false;
	memcpy(cdst, remotePage, sz);
	cdst += sz;
	corig += sz;

	for(size_t i=1; i<npages; ++i) {
		sz = std::min(n, (size_t)PAGE_SIZE);
		remotePage = remoteToKernel((uint64_t)corig, remote);
		if(!remotePage)
			return false;
		memcpy(cdst, remotePage, sz);
		cdst += sz;
		corig += sz;
	}

	return true;
}
