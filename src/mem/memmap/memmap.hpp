#ifndef MEMMAP_HPP
#define MEMMAP_HPP

#include <boot/bootdata.hpp>

class MemoryMap {
private:
	uint64_t entries = 0;
	stivale2_mmap_entry* data = nullptr;

public:
	MemoryMap(stivale2_struct* bootData);

	typedef const stivale2_mmap_entry* iterator;
	inline iterator begin() const { return data; }
	inline iterator end() const { return data+entries; }
	inline bool empty() const { return !data; }
	inline uint64_t getn() const { return entries; }

	void move(stivale2_mmap_entry*);
};

#endif
