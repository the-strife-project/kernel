#include "bootstrap.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <panic/bruh.hpp>

PID psnsPID = 0;
PID vfsPID = 0;

void Bootstrap::bootstrap() {
	// --- PSNS ---
	psnsPID = run("PSNS", BootModules::MODULE_ID_PSNS);
	auto pp = getTask(psnsPID);
	pp.acquire();
	Task* psns = pp.get()->task;
	setOrigRunning(psnsPID);
	pp.release();
	psns->dispatchSaving();
	printf("[OK]\n");

	// --- term ---
	PID termPID = run("term", BootModules::MODULE_ID_TERM);
	pp = getTask(termPID);
	pp.acquire();
	Task* term = pp.get()->task;

	// Map framebuffer
	size_t fbsize = FB_ROWS * FB_COLS * 2;
	size_t fbpages = NPAGES(fbsize);
	uint64_t fb = term->getASLR().get(fbpages, GROWS_UPWARD, PAGE_SIZE);
	uint64_t flags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	term->getPaging().map(fb, PHYS_VIDEO_BASE, fbsize, flags);

	// Map cursor sync for kernel
	uint64_t vkcursor = PublicMM::calloc(); // Used by the kernel
	uint64_t kcursor = kpaging.getPhys(vkcursor); // Its physical
	uint64_t pkcursor = term->getASLR().get(1, GROWS_UPWARD, PAGE_SIZE); // Process
	term->getPaging().map(pkcursor, kcursor, PAGE_SIZE, flags);
	nowSyncWithTerm((uint64_t*)vkcursor);

	// Send framebuffer address and cursor sync
	term->getState().regs.rdi = fb;
	term->getState().regs.rsi = pkcursor;

	setOrigRunning(termPID);
	pp.release();
	term->dispatchSaving();
	printf("[OK]\n");

	// --- PCI ---
	PID pciPID = run("PCI", BootModules::MODULE_ID_PCI);
	pp = getTask(pciPID);
	pp.acquire();
	Task* pci = pp.get()->task;
	setOrigRunning(pciPID);
	pp.release();
	pci->dispatchSaving();
	printf("[OK]\n");

	// --- AHCI ---
	PID ahciPID = run("AHCI", BootModules::MODULE_ID_AHCI);
	pp = getTask(ahciPID);
	pp.acquire();
	Task* ahci = pp.get()->task;
	setOrigRunning(ahciPID);
	pp.release();
	ahci->dispatchSaving();
	printf("[OK]\n");

	// --- ramblock ---
	PID ramblockPID = run("ramblock", BootModules::MODULE_ID_RAMBLOCK);
	pp = getTask(ramblockPID);
	pp.acquire();
	Task* ramblock = pp.get()->task;
	setOrigRunning(ramblockPID);
	pp.release();
	ramblock->dispatchSaving();
	printf("[OK]\n");

	// --- block ---
	PID blockPID = run("block", BootModules::MODULE_ID_BLOCK);
	pp = getTask(blockPID);
	pp.acquire();
	Task* block = pp.get()->task;
	setOrigRunning(blockPID);
	pp.release();
	block->dispatchSaving();
	printf("[OK]\n");

	// --- ISO9660 ---
	PID isoPID = run("ISO9660", BootModules::MODULE_ID_ISO9660);
	pp = getTask(isoPID);
	pp.acquire();
	Task* iso = pp.get()->task;
	iso->getState().regs.rdi = true; // Let it know it's from bootstrapping
	setOrigRunning(isoPID);
	pp.release();
	iso->dispatchSaving();
	printf("[OK]\n");

	// --- STRIFEFS ---
	PID fsPID = run("StrifeFS", BootModules::MODULE_ID_STRIFEFS);
	pp = getTask(fsPID);
	pp.acquire();
	Task* fs = pp.get()->task;
	setOrigRunning(fsPID);
	pp.release();
	fs->dispatchSaving();
	printf("[OK]\n");

	// --- VFS ---
	vfsPID = run("VFS", BootModules::MODULE_ID_VFS);
	pp = getTask(vfsPID);
	pp.acquire();
	Task* vfs = pp.get()->task;
	vfs->getState().regs.rdi = true; // Booting from ISO
	setOrigRunning(vfsPID);
	pp.release();
	vfs->dispatchSaving();
	printf("[OK]\n");

	// --- INIT ---
	printf("\nGoing for init\n");
	PID initPID = run(nullptr, BootModules::MODULE_ID_INIT, true);
	sched.add(initPID);
}
