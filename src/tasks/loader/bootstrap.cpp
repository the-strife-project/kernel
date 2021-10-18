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
#define LOADER_BASE (1 << 20)
// ↓ 1GB ↓
#define LOADER_HEAP (1 << 30)
// ↓ 64 GB ↓
#define LOADER_STACK (64 * (1ull << 30))

PID LOADER_PID;

void Loader::bootstrapLoader() {
	uint64_t rawbegin = stivale2Modules::loader_beg;
	uint64_t rawend = stivale2Modules::loader_end;

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
	uint64_t size = rawend - begin;


	// --- Paging object ---
	Paging paging;
	paging.setData((Paging::PML4E*)PMM::calloc());
	// Add the kernel global entry (last PML4E), in case the TLB gets cleared
	paging.getData()[PAGE_ENTRIES - 1] = kpaging.getData()[PAGE_ENTRIES - 1];


	// --- Mapping ---
	Paging::PageMapping map(paging, LOADER_BASE);
	map.setUser();
	size_t npages = size / PAGE_SIZE;
	size_t lastpagesz = size % PAGE_SIZE;
	if(lastpagesz)
		++npages;
	else
		lastpagesz = PAGE_SIZE;

	// --- Copy executable pages ---
	// Copy all pages but the last one
	uint64_t orig = rawbegin + sizeof(Header);
	while(--npages) {
		uint64_t page = PMM::alloc();	// No need to calloc() here.
		memcpy((void*)page, (void*)orig, PAGE_SIZE);
		map.map4K(page);
		orig += PAGE_SIZE;
	}
	// Last page
	uint64_t page = PMM::calloc();
	memcpy((void*)page, (void*)orig, lastpagesz);
	map.map4K(page);

	// Map stack (no need to map heap, it is done on demand)
	auto stackFlags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	paging.map(LOADER_STACK - PAGE_SIZE, PMM::calloc(), PAGE_SIZE, stackFlags);

	// ASLR object
	ASLR aslr;
	aslr.set(LOADER_BASE, npages);
	aslr.set(LOADER_STACK, MAX_STACK_PAGES);
	aslr.set(LOADER_HEAP, MAX_HEAP_PAGES);

	// Task
	LoaderInfo loaderInfo(paging, aslr, LOADER_BASE, LOADER_HEAP, LOADER_STACK);
	Task* task = (Task*)PMM::calloc();
	*task = Task(loaderInfo, LOADER_BASE + entrypoint);

	Scheduler::SchedulerTask schedTask;
	schedTask.paging = paging;
	schedTask.task = task;
	// General task (https://bit.ly/3xXdHUT)

	LOADER_PID = assignPID(schedTask);

	// Run!
	running[whoami()] = LOADER_PID;
	bochs();
	schedTask.task->dispatchSaving();
	printf("Back from loader!");
	while(true);
}
