#ifndef ASLR_HPP
#define ASLR_HPP

#include <mem/paging/paging.hpp>
#include <klibc/klibc.hpp>
#include <pair>

// ↓ From low to high mem (heap)
#define GROWS_UPWARD   false
// ↓ From high to low mem (stack)
#define GROWS_DOWNWARD true

#define ASLR_BASE_ID 0

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
	PrivList<std::pair<size_t, uint64_t>> ids; // Should be a vector, but meh, no biggie

public:
	uint64_t get(size_t max_pages, bool direction, uint64_t alignment, bool doNotPanic=false);
	void set(uint64_t addr, size_t max_pages);
	void free(uint64_t addr);

	void setID(size_t id, size_t npages);
	uint64_t getFromID(size_t id, size_t npages); // Syscall handler

	// Debugging
	inline const PrivList<std::pair<size_t, uint64_t>>& getIDs() const { return ids; }
};

#endif
