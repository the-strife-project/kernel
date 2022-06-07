// This file is injected to paging.hpp
// No 1GB pages!

class PageMapping {
private:
	uint64_t pml4_i, pdp_i, pd_i, pt_i;
	PML4E* pml4e = nullptr;
	PDPE* pdpe = nullptr;
	PDE* pde = nullptr;
	PTE* pte = nullptr;

	void nextPML4E();
	void nextPDPE();
	void nextPDE(bool huge);
	void nextPTE();

	bool global = false;
	bool ro = false;
	bool nx = false;
	bool user = false;
	bool present = true;
	bool pcd = false;

	PML4E* data;
	uint64_t virt;

public:
	PageMapping(Paging, uint64_t virt);

	inline void setGlobal() { global = true; }
	inline void setRO() { ro = true; }
	inline void setNX() { nx = true; }
	inline void setUser() { user = true; }
	inline void setNotPresent() { present = false; }
	inline void setPCD() { pcd = true; }

	void map4K(uint64_t phys);
	void map2M(uint64_t phys);
};
