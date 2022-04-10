#include "ht64.hpp"
#include <panic/bruh.hpp>
#include <algorithm>

// Public insertion routine
void HT64::add(uint64_t key, uint64_t val) {
	// Fast check, just in case
	if(key == 0)
		bruh(Bruh::HT64_NULL_KEY);

	// Used percentage?
	if(!buckets) {
		more();
	} else {
		size_t used = (filledBuckets * 100) / buckets;
		if(used >= MAX_USAGE)
			more();
	}

	BucketNode bn;
	bn.key = key;
	bn.val = val;
	insert(bn);
}

void HT64::more() {
	// TODO one could crash the system calling this

	// Doubling sounds about right
	// Allocation depends on "state"
	uint64_t virt = space + (state ? howMuch/2 : 0);
	for(size_t i=0; i<pages*2; ++i) {
		// With calloc(), BucketNode initialization is given for free
		kpaging.map(virt, PMM::calloc(), PAGE_SIZE, FLAGS);
		virt += PAGE_SIZE;
	}
	pages *= 2;

	auto prevBuckets = buckets;
	buckets += BUCKETS_PER_PAGE;
	filledBuckets = 0;

	// Make the switch
	state = !state;
	BucketNode* odata = data;
	data = state ? dataHi : dataLo;

	// Just insert all from !state
	for(size_t i=0; i<prevBuckets; ++i)
		insert(odata[i]);

	// Free odata
	virt = space + (state ? 0 : howMuch/2);
	for(size_t i=0; i<(pages/2); ++i) {
		uint64_t phys = kpaging.getPhys(virt);
		kpaging.unmap(virt);
		PMM::free(phys);
	}
}

// Private insertion routine
void HT64::insert(const BucketNode& bn) {
	// This method can't fail, so...
	++filledBuckets;

	auto h = hash(bn.key);
	auto origh = h;

	// Nope. Here we go
	BucketNode cbn = bn;
	bool first = true;
	while(first || h != origh) {
		if(!data[h].key) {
			// Free!
			data[h] = cbn;
			return;
		}

		if(first) first = false;

		if(cbn.psl > data[h].psl)
			std::swap(cbn, data[h]);

		if(++h == buckets)
			h = 0;

		++cbn.psl;
	}

	// Looks like second pass ended. So there was no space for the node
	// This is impossible since there's a max usage
	bruh(Bruh::HT64_NO_SPACE);
}
