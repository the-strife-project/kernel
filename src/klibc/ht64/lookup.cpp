#include "ht64.hpp"
#include <panic/bruh.hpp>

// Private lookup routine
const HT64::BucketNode* HT64::lookup(uint64_t key) const {
	// Linear probing, best for cache afaik
	auto h = hash(key);
	auto origh = h;

	bool first = true;
	size_t psl = 0;
	while(first || h != origh) {
		if(data[h].key == key)
			return &data[h];

		// Two cases: empty or higher PSL
		if(!data[h].key || data[h].psl > psl)
			return nullptr;

		if(first) first = false;

		// Two passes, same as insert()
		if(++h == buckets)
			h = 0;

		++psl;
	}

	bruh(Bruh::HT64_FULL_ON_LOOKUP);
}
