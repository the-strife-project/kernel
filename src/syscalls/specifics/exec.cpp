#include "../syscalls.hpp"
#include <tasks/PIDs/PIDs.hpp>

static Spinlock loaderLock;
extern "C" [[noreturn]] void execSwitchStack(PID, uint64_t, size_t, uint64_t, size_t, uint64_t);

void exec(PID parent, uint64_t buffer, size_t sz, uint64_t runtime, size_t rtsz) {
	// Check valid values
	if(sz >= Loader::MAX_ELF_SIZE)
		return;
	if(PAGEOFF(buffer))
		return; // Must be page-aligned
	if(PAGEOFF(runtime))
		return; // Same

	// Start checking if the buffer is correct
	auto parentpp = getTask(parent);
	auto& parentst = *(parentpp.get());

	size_t npages = NPAGES(sz);
	for(size_t i=0; i<npages; ++i)
		if(!parentst.paging.getPhys(buffer + i * PAGE_SIZE))
			return;

	// Runtime too
	npages = NPAGES(rtsz);
	for(size_t i=0; i<npages; ++i)
		if(!parentst.paging.getPhys(runtime + i * PAGE_SIZE))
			return;

	// From this point on, this function doesn't return
	// That's why, first of all, let's save the state
	parentst.task->saveStateSyscall();

	execSwitchStack(parent, buffer, sz, runtime, sz, loaderStacks[whoami()]);
}

extern "C" void execPartTwo(PID parent, uint64_t buffer, size_t sz, uint64_t runtime, size_t rtsz) {
	// We're now on a public stack, independent of the syscall that called this
	auto parentpp = getTask(parent);
	auto& parentst = *(parentpp.get());
	size_t npages = NPAGES(sz);

	// Acquire the loader
	loaderLock.acquire();
	auto loaderpp = getTask(Loader::LOADER_PID);
	loaderpp.acquire();

	// Now, start steal pages from process to loader
	for(size_t i=0; i<npages; ++i) {
		uint64_t phys = parentst.paging.getPhys(buffer);
		parentst.paging.unmap(buffer);
		parentst.task->decUsedPages();
		// phys is now an unlinked page
		if(!Loader::movePage(phys, i))
			bruh(Bruh::LOADER_MOVE_PAGE);

		buffer += PAGE_SIZE;
	}

	// That's all for the parent, keep it locked for later
	setOrigRunning(Loader::LOADER_PID);
	// Dispatch the loader
	Task* loadert = loaderpp.get()->task;
	loadert->getState().regs.rax = sz;
	loaderpp.release();
	loadert->dispatchSaving();

	// Program has been loaded, grab the values
	loaderpp.acquire();
	Loader::freeELF();
	size_t child = Loader::last_pid;
	size_t error = Loader::last_err;
	size_t entry = Loader::last_entry;
	setOrigRunning(NULL_PID);
	loaderpp.release();
	loaderLock.release();

	if(!error) {
		// Acquire child
		auto childpp = getTask(child);
		childpp.acquire();
		auto& childst = *(childpp.get());

		// Put runtime information
		npages = NPAGES(rtsz);
		uint64_t rtbase = childst.task->getASLR().get(npages, GROWS_UPWARD, PAGE_SIZE);
		uint64_t rtcur = rtbase;
		for(size_t i=0; i<npages; ++i) {
			uint64_t phys = parentst.paging.getPhys(runtime + i * PAGE_SIZE);
			parentst.paging.unmap(runtime + i * PAGE_SIZE);
			parentst.task->decUsedPages();
			// phys is now an unlinked page
			const size_t flags =
				Paging::MapFlag::NX |
				Paging::MapFlag::USER;
			childst.task->getPaging().map(rtcur, phys, PAGE_SIZE, flags);
			childst.task->incUsedPages();

			rtcur += PAGE_SIZE;
		}

		// And set the values
		childst.parent = parent;
		childst.uid = parentst.uid; // Inherit UID
		childst.task->jump(entry);
		childst.task->getState().regs.rdi = rtbase;
		childpp.release();
		parentpp.release();

		// It's now free to run
		sched.add(child);

		// Acquire parent again
		parentpp.acquire();
		if(parentpp.isNull()) {
			// Damn
			parentpp.release();
			schedule();
		}

		// Add child
		Scheduler::SchedulerTask::Child childEntry;
		childEntry.pid = child;
		parentst.children.push_back(childEntry);
		// Loader error and PID of child
		parentst.lastLoaderError = error;
		parentst.task->getState().regs.rax = child;
		parentpp.release();
	} else {
		parentpp.acquire();
		if(parentpp.isNull()) {
			// Damn
			parentpp.release();
			schedule();
		}

		parentst.lastLoaderError = error;
		parentpp.release();
	}

	// It's free to run now as well
	sched.add(parent);

	// That's it!
	schedule();
}
