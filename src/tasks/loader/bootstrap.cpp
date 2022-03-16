#include "loader.hpp"
#include <boot/modules/modules.hpp>
#include <panic/panic.hpp>
#include "../constants.hpp"
#include "../task/task.hpp"
#include "../scheduler/scheduler.hpp"
#include "../PIDs/PIDs.hpp"
#include <CPU/SMP/SMP.hpp>

#define SUS_MAGIC 0x5355537F
// ↓ 1MB ↓
#define LOADER_BASE (1ull << 20)
// ↓ 1GB ↓
#define LOADER_HEAP (1ull << 30)
// ↓ 64 GB ↓
#define LOADER_STACK (64 * (1ull << 30))

PID Loader::LOADER_PID;

void howManyPages(size_t size, size_t& npages, size_t& lastpagesz) {
	npages = size / PAGE_SIZE;
	lastpagesz = size % PAGE_SIZE;
	if(lastpagesz)
		++npages;
	else
		lastpagesz = PAGE_SIZE;
}

static void _mapInLoader(Paging paging, uint64_t begin, size_t npages, size_t lastpagesz, uint64_t where) {
	Paging::PageMapping map(paging, where);
	map.setUser();

	// Copy all pages but the last one
	while(--npages) {
		uint64_t page = PMM::alloc();	// No need to calloc() here.
		memcpy((void*)page, (void*)begin, PAGE_SIZE);
		map.map4K(page);
		begin += PAGE_SIZE;
	}

	// Last page
	uint64_t page = PMM::calloc();
	memcpy((void*)page, (void*)begin, lastpagesz);
	map.map4K(page);
}

static uint64_t mapInLoader(Paging paging, uint64_t begin, size_t size, ASLR& aslr, uint64_t where) {
	size_t npages, lastpagesz;
	howManyPages(size, npages, lastpagesz);
	aslr.set(where, npages);
	_mapInLoader(paging, begin, npages, lastpagesz, where);
	return where;
}

void Loader::bootstrapLoader() {
	uint64_t rawbegin = BootModules::begins[BootModules::MODULE_ID_LOADER];
	uint64_t rawsize = BootModules::sizes[BootModules::MODULE_ID_LOADER];
	if(!rawbegin)
		panic(Panic::NO_LOADER);

	// Parse SUS executable
	struct Header {
		uint32_t magic;
		uint64_t entrypoint;
	} __attribute__((packed));
	Header* header = (Header*)rawbegin;

	if(header->magic != SUS_MAGIC)
		panic(Panic::LOADER_NOT_SUS);

	uint64_t entrypoint = header->entrypoint;
	uint64_t begin = rawbegin + sizeof(Header);
	uint64_t size = rawsize - sizeof(Header);


	// Paging object
	Paging paging;
	paging.setData((Paging::PML4E*)PMM::calloc());
	// Add the kernel global entry (last PML4E), in case the TLB gets flushed
	paging.getData()[PAGE_ENTRIES - 1] = kpaging.getData()[PAGE_ENTRIES - 1];

	// ASLR object
	ASLR aslr;

	// Map program
	mapInLoader(paging, begin, size, aslr, LOADER_BASE);

	// Map stack (no need to map heap, it is done on demand)
	auto stackFlags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	paging.map(LOADER_STACK - PAGE_SIZE, PMM::calloc(), PAGE_SIZE, stackFlags);
	aslr.set(LOADER_STACK, MAX_STACK_PAGES);
	aslr.set(LOADER_HEAP, MAX_HEAP_PAGES);

	// Mount the stdlib, first thing the loader has to parse
	begin = BootModules::begins[BootModules::MODULE_ID_STDLIB];
	size = BootModules::sizes[BootModules::MODULE_ID_STDLIB];
	if(!begin)
		panic(Panic::NO_STDLIB);
	if(size > MAX_ELF_SIZE)
		panic(Panic::BOOTSTRAP_ELF_TOO_BIG);
	aslr.set(ELF_BASE, MAX_ELF_SIZE / PAGE_SIZE);

	size_t npages, lastpagesz;
	howManyPages(size, npages, lastpagesz);

	_mapInLoader(paging, begin, npages, lastpagesz, ELF_BASE);

	// Task
	LoaderInfo loaderInfo(paging, aslr, LOADER_BASE, LOADER_HEAP, LOADER_STACK);
	Task* task = (Task*)PMM::calloc();
	*task = Task(loaderInfo, LOADER_BASE + entrypoint);

	// Parameters
	task->getState().regs.rdi = ELF_BASE;
	task->getState().regs.rsi = size;

	Scheduler::SchedulerTask schedTask;
	schedTask.paging = paging;
	schedTask.task = task;
	// General task (https://bit.ly/3xXdHUT)

	Loader::LOADER_PID = assignPID(schedTask);

	// Run!
	running[whoami()] = Loader::LOADER_PID;
	schedTask.task->dispatchSaving();

	// How did it go?
	if(Loader::last_err)
		panic(Panic::BAD_STDLIB);

	Loader::freeELF();
}
