#include "bootstrap.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/PIDs/PIDs.hpp>
//#include <CPU/SMP/SMP.hpp>
#include <panic/bruh.hpp>

void Bootstrap::bootstrap() {
	// --- TERM ---
	PID termPID = run("term", BootModules::MODULE_ID_TERM);
	auto pp = getTask(termPID);
	pp.acquire();
	Task* term = pp.get()->task;

	// Map framebuffer
	size_t fbsize = FB_ROWS * FB_COLS * 2;
	size_t fbpages = (fbsize + PAGE_SIZE - 1) / PAGE_SIZE;
	uint64_t fb = term->getASLR().get(fbpages, GROWS_UPWARD, PAGE_SIZE);
	uint64_t flags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	term->getPaging().map(fb, PHYS_VIDEO_BASE, fbsize, flags);

	// Map cursor sync for kernel
	uint64_t kcursor = VMM::Private::calloc();
	uint64_t pkcursor = term->getASLR().get(1, GROWS_UPWARD, PAGE_SIZE);
	term->getPaging().map(pkcursor, kcursor, 4096, flags);
	nowSyncWithTerm((uint64_t*)kcursor);

	// Send framebuffer address and cursor sync
	term->getState().regs.rdi = fb;
	term->getState().regs.rsi = pkcursor;

	thisCoreIsNowRunning(termPID);
	pp.release();
	term->dispatchSaving();
	printf("[OK]\n");

	// --- PCI ---
	PID pciPID = run("PCI", BootModules::MODULE_ID_PCI);
	pp = getTask(pciPID);
	pp.acquire();
	Task* pci = pp.get()->task;
	thisCoreIsNowRunning(pciPID);
	pp.release();
	pci->dispatchSaving();
	printf("[OK]\n");
}
