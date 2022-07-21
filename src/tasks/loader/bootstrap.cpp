#include "loader.hpp"
#include <boot/modules/modules.hpp>
#include <panic/panic.hpp>
#include "../constants.hpp"
#include "../task/task.hpp"
#include "../scheduler/scheduler.hpp"
#include "../PIDs/PIDs.hpp"
#include <CPU/SMP/SMP.hpp>

#define SUS_MAGIC 0x5355537F
// I wanted loaders's base to be 1MB, but setting it statically in the ELF
//   (and therefore in the SUS) is way more comfortable. I've had some issues
//   with this, so I do it this way. As the SUS is a flat binary, 1MB would
//   make the file >1MB, which increases a lot the size of the ISO. For this
//   very reason, I set it to 4KB, which should be fine.
#define LOADER_BASE 0
#define LOADER_SKIP 0x1000
// ↓ 1GB ↓
#define LOADER_HEAP (1ull << 30)
// ↓ 64 GB ↓
#define LOADER_STACK (64ull * (1ull << 30))

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
		uint64_t page = PhysMM::alloc();	// No need to calloc() here.
		memcpy((void*)page, (void*)begin, PAGE_SIZE);
		map.map4K(page);
		begin += PAGE_SIZE;
	}

	// Last page
	uint64_t page = PhysMM::calloc();
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
	uint64_t begin = rawbegin + sizeof(Header) + LOADER_SKIP;
	uint64_t size = rawsize - (sizeof(Header) + LOADER_SKIP);
	uint64_t loaderSize = size;


	// Paging object
	Paging paging;
	paging.setData((Paging::PML4E*)PhysMM::calloc());
	// Add the kernel global entry (last PML4E), in case the TLB gets flushed
	paging.getData()[PAGE_ENTRIES - 1] = kpaging.getData()[PAGE_ENTRIES - 1];
	// No need to worry about generalTask, loader doesn't RPC

	// ASLR object
	ASLR aslr;

	// Map program
	mapInLoader(paging, begin, size, aslr, LOADER_SKIP);

	// Map stack (no need to map heap, it is done on demand)
	auto stackFlags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	paging.map(LOADER_STACK - PAGE_SIZE, PhysMM::calloc(), PAGE_SIZE, stackFlags);
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
	Task* task = (Task*)PhysMM::calloc();
	*task = Task(loaderInfo, LOADER_BASE + entrypoint, (uint64_t)task);

	// Parameters
	task->getState().regs.rdi = ELF_BASE;
	task->getState().regs.rsi = size;
	// Loader is the only task not preemptable, so clear IF
	task->getState().rflags &= ~(1 << RFLAGS::IF);

	// Used pages
	task->incUsedPages(NPAGES(loaderSize));
	task->incUsedPages(); // Stack

	Scheduler::SchedulerTask schedTask;
	schedTask.task = task;
	schedTask.paging = paging;

	Loader::LOADER_PID = assignPID(schedTask);
	task->setAs(Loader::LOADER_PID);

	// Run!
	setOrigRunning(Loader::LOADER_PID);
	schedTask.task->dispatchSaving();

	auto pp = getTask(Loader::LOADER_PID);
	pp.acquire();

	// How did it go?
	if(Loader::last_err) {
		panic(Panic::BAD_STDLIB, DO_NOT_PANIC);
		printf("\nLoader error: 0x%x\n", Loader::last_err);
		hlt();
	}

	Loader::freeELF();
	pp.release();
}
