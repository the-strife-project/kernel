#include "task.hpp"
#include <algorithm>
#include <tasks/PIDs/PIDs.hpp>

static void* remoteToKernel(void* ptr, Paging remote) {
	uint64_t iptr = (uint64_t)ptr;
	uint64_t ret = remote.getPhys(iptr & ~0xFFF);
	if(!ret)
		return nullptr;
	ret += iptr & 0xFFF;
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
	size_t sz = std::min(n, PAGE_SIZE - (((size_t)orig) & 0xFFF));
	void* remotePage = remoteToKernel(corig, remote);
	if(!remotePage)
		return false;
	memcpy(cdst, remotePage, sz);
	cdst += sz;
	corig += sz;

	for(size_t i=1; i<npages; ++i) {
		sz = std::min(n, (size_t)PAGE_SIZE);
		remotePage = remoteToKernel(corig, remote);
		if(!remotePage)
			return false;
		memcpy(cdst, remotePage, sz);
		cdst += sz;
		corig += sz;
	}

	return true;
}
