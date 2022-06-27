#ifndef HT64_HPP
#define HT64_HPP

#include <common.hpp>
#include <klibc/klibc.hpp>

// Hash Table: uint64_t -> uint64_t
// Very specific case, I know. It's for published names

// Assumption: no key is equal to 0
// Assumption: no value is equal to 0

/*
	Here's the idea. This is for kernel use only.
	Since consecutive page allocation is not implemented inside the kernel,
	  and since this class will pretty much be a singleton, it needs a free
	  region in virtual memory. 1 GB would be nice.
	In the use case (published names), it must be in the higher half so that
	  the pages are global and "public" (mapped to all processes).

	This implies that no movement is necessary when resizing. No new realloc
	  takes place, only the next page is allocated. And that's an issue for
	  rehashing. For this, only half of the space is used at any point,
	  and it's switched back and forth between the two every rehash.
*/

// This is Robin Hood Hashing
// https://programming.guide/robin-hood-hashing.html

class HT64 {
private:
	struct BucketNode {
		// Designed so that calloc() initializes the objects
		uint64_t key = 0;
		uint64_t val = 0;
		size_t psl = 0;
	};

	static const size_t BUCKETS_PER_PAGE = PAGE_SIZE / sizeof(BucketNode);

	uint64_t space = 0; // Base page of free consecutive virtual memory
	size_t howMuch = 0; // How many bytes?

	size_t pages = 0; // How many pages are allocated for data
	size_t buckets = 0; // always = pages * BUCKETS_PER_PAGE

	// This is (BucketNode*)space
	BucketNode* dataLo = nullptr;
	// And this is (BucketNode*)(space + howMuch/2)
	BucketNode* dataHi = nullptr;
	// Are we on low or high memory?
	bool state = false; // Low by default
	// state ? dataHi : dataLo
	BucketNode* data = nullptr;

	size_t filledBuckets = 0;

	static const size_t MAX_USAGE = 80; // 80% of filled buckets at max

	static const size_t FLAGS = Paging::MapFlag::GLOBAL | Paging::MapFlag::NX;
	void more();
	void insert(const BucketNode& bn);
	const BucketNode* lookup(size_t key) const;

	// This is very suboptimal
	inline size_t hash(uint64_t key) const { return key % buckets; }

public:
	HT64() = default;
	HT64(uint64_t space_, size_t howMuch_) {
		space = space_;
		howMuch = howMuch_;
		dataLo = (BucketNode*)space;
		dataHi = (BucketNode*)(space + howMuch/2);
		data = dataLo;

		// First allocation
		pages = 1;
		buckets = BUCKETS_PER_PAGE;
		kpaging.map((uint64_t)data, PMM::calloc(), PAGE_SIZE, FLAGS);
	}

	inline size_t size() const { return filledBuckets; }
	void add(uint64_t key, uint64_t val);
	inline bool has(uint64_t key) const { return get(key); }
	inline uint64_t get(uint64_t key) const {
		const BucketNode* bn = lookup(key);
		if(!bn) return 0;
		return bn->val;
	}
	void erase(uint64_t key);
};

#endif
