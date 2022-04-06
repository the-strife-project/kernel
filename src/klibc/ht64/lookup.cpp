#include "ht64.hpp"
#include <panic/bruh.hpp>

// Private lookup routine
const HT64::BucketNode* HT64::lookup(uint64_t key) const {
	// Linear probing, best for cache afaik
	auto h = hash(key);
	auto origh = h;

	if(data[h].key == key)
		return &data[h];
	++h;

	uint64_t psl = 0;
	while(true) {
		if(data[h].key == key)
			return &data[h];

		// Two cases: empty or higher PSL
		if(!data[h].key || data[h].psl > psl)
			return nullptr;

		// Two passes, same as insert()
		if(h == origh) {
			// Second pass complete
			// This would imply that the hash table is full, which is impossible
			bruh(Bruh::HT64_FULL_ON_LOOKUP);
		} else if(h == buckets) {
			// First pass complete, start the second
			h = 0;
		} else {
			++h;
		}

		++psl;
	}
}
