#ifndef ASLR_HPP
#define ASLR_HPP

#include <mem/paging/paging.hpp>
#include <klibc/klibc.hpp>

#define GROWS_UPWARD   false
#define GROWS_DOWNWARD true
#define DO_NOT_PANIC   true

class ASLR {
private:
	struct Node {
		uint64_t begin;
		uint64_t end;

		Node() = default;
		Node(uint64_t begin, uint64_t end)
			: begin(begin), end(end)
		{}
	};

	PrivList<Node> list;

public:
	uint64_t get(size_t max_pages, bool direction, uint64_t alignment, bool doNotPanic=false);
	void free(uint64_t addr);
};

#endif
