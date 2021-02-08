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

	inline bool isPresent() { return present; }
	inline void setPresent(bool v=true) { present = (uint64_t)v; }
	inline void setRO() { rw = 0; }
	inline void setUser() { allowUser = 1; }
	inline void setNext(uint64_t v) { next = v; }
	inline uint64_t getNext() { return next; }
	inline void setExecutable(bool v=true) { nx = (uint64_t)(!v); }
	inline void setHuge() { huge = 1; }
	inline bool getHuge() { return huge; }
	inline void setGlobal() { global = 1; }
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
	uint64_t avl2 : 3;
	uint64_t phys : 40;
	uint64_t avl : 7;
	uint64_t pke : 4;
	uint64_t nx : 1;

public:
	inline PTE()
		: present(0), rw(1), allowUser(0), writethrough(0), cacheDisable(0),
		  accessed(0), dirty(0), pat(0), global(0), avl2(0), phys(0), avl(0), pke(0), nx(0)
	{}

	inline bool isPresent() { return present; }
	inline void setPresent(bool v=true) { present = (uint64_t)v; }
	inline void setRO() { rw = 0; }
	inline void setUser() { allowUser = 1; }
	inline void setGlobal() { global = 1; }
	inline void setPhys(uint64_t v) { phys = v; }
	inline uint64_t getPhys() { return phys; }
	inline void setExecutable(bool v=true) { nx = (uint64_t)(!v); }
} __attribute__((packed));
