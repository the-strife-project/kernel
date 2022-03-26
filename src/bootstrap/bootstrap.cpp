#include "bootstrap.hpp"
#include <boot/modules/modules.hpp>
#include <tasks/PIDs/PIDs.hpp>

void Bootstrap::bootstrap() {
	// --- TERM ---
	PID termPID = run("term", BootModules::MODULE_ID_TERM);
	Task* term = getTask(termPID).task;

	// Map framebuffer
	size_t fbsize = FB_ROWS * FB_COLS * 2;
	size_t fbpages = (fbsize + PAGE_SIZE - 1) / PAGE_SIZE;
	uint64_t fb = term->getASLR().get(fbpages, GROWS_UPWARD, PAGE_SIZE);
	uint64_t flags = Paging::MapFlag::USER | Paging::MapFlag::NX;
	term->getPaging().map(fb, PHYS_VIDEO_BASE, fbsize, flags);

	// Send current cursor position
	size_t row, col;
	getRC(row, col);
	term->getState().regs.rdi = fb;
	term->getState().regs.rsi = row;
	term->getState().regs.rdx = col;

	//resetKernelTerm(); // TODO uncomment this soon
}



