// This file is injected into the public part of the class Paging

class PML4E {
	private:
	uint64_t present : 1;
	uint64_t rw : 1;
	uint64_t allowUser : 1;
	uint64_t writethrough : 1;
	uint64_t cacheDisable : 1;
	uint64_t accessed : 1;
	uint64_t ign : 1;
	uint64_t huge : 1;	// Only for PDPE and PDE!
	uint64_t global : 1;	// Only if huge!
	uint64_t avl2 : 3;
	uint64_t next : 40;
	uint64_t avl : 11;
	uint64_t nx : 1;

public:
	inline PML4E()
		: present(0), rw(1), allowUser(0), writethrough(0), cacheDisable(0),
		  accessed(0), ign(0), huge(0), global(0), avl2(0), next(0), avl(0), nx(0)
	{}

	inline void setPresent(bool v=true) { present = (uint64_t)v; }
	inline bool isPresent() { return present; }
	inline void setRO() { rw = 0; }
	inline bool isRO() { return !rw; }
	inline void setUser() { allowUser = 1; }
	inline bool isUser() { return allowUser; }
	inline void setNext(uint64_t v) { next = v; }
	inline uint64_t getNext() { return next; }
	inline void setNX() { nx = 1; }
	inline bool isNX() { return nx; }
	inline void setHuge() { huge = 1; }
	inline bool isHuge() { return huge; }
	inline void setGlobal() { global = 1; }
	inline bool isGlobal() { return global; }
} __attribute__((packed));

typedef PML4E PDPE;
typedef PML4E PDE;


class PTE {
private:
	uint64_t present : 1;
	uint64_t rw : 1;
	uint64_t allowUser : 1;
	uint64_t writethrough : 1;
	uint64_t cacheDisable : 1;
	uint64_t accessed : 1;
	uint64_t dirty : 1;
	uint64_t pat : 1;
	uint64_t global : 1;
	uint64_t alloc0 : 3;	// Available, used for the allocator
	uint64_t phys : 40;
	uint64_t avl : 2;		// Available
	uint64_t alloc1 : 5;	// Available, used for the allocator
	uint64_t pke : 4;
	uint64_t nx : 1;

public:
	inline PTE()
		: present(0), rw(1), allowUser(0), writethrough(0), cacheDisable(0),
		  accessed(0), dirty(0), pat(0), global(0), alloc0(0), phys(0), avl(0), alloc1(0), pke(0), nx(0)
	{}

	inline void setPresent(bool v=true) { present = (uint64_t)v; }
	inline bool isPresent() { return present; }
	inline void setRO() { rw = 0; }
	inline bool isRO() { return !rw; }
	inline void setUser() { allowUser = 1; }
	inline bool isUser() { return allowUser; }
	inline void setGlobal() { global = 1; }
	inline bool isGlobal() { return global; }
	inline void setPhys(uint64_t v) { phys = v; }
	inline uint64_t getPhys() { return phys; }
	inline void setNX() { nx = 1; }
	inline bool isNX() { return nx; }
	inline void setPCD() { cacheDisable = 1; }
	inline bool isPCD() { return cacheDisable; }

	// OS-dependent
	inline void setUsedChunks(uint16_t x) {
		alloc0 = x & 0b111;
		alloc1 = (x >> 3) & 0b11111;
	}

	inline uint16_t getUsedChunks() {
		return alloc0 | (alloc1 << 3);
	}

	inline void incUsedChunks() { setUsedChunks(getUsedChunks()+1); }
	inline void decUsedChunks() { setUsedChunks(getUsedChunks()-1); }
} __attribute__((packed));
