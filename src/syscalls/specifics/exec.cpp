#include "../syscalls.hpp"
#include <tasks/PIDs/PIDs.hpp>

static Spinlock loaderLock;
extern "C" [[noreturn]] void execSwitchStack(PID, uint64_t, size_t, uint64_t);

void exec(PID parent, uint64_t buffer, size_t sz) {
	// Check valid values
	if(sz >= Loader::MAX_ELF_SIZE)
		return;
	if(PAGEOFF(buffer))
		return; // Must be page-aligned

	// Start checking if the buffer is correct
	auto parentpp = getTask(parent);
	auto& parentst = *(parentpp.get());

	size_t npages = (sz + PAGE_SIZE - 1) / PAGE_SIZE;
	for(size_t i=0; i<npages; ++i) {
		uint64_t phys = parentst.paging.getPhys(buffer + i * PAGE_SIZE);
		if(!phys)
			return;
	}

	// From this point on, this function doesn't return
	// That's why, first of all, let's save the state
	parentst.task->saveStateSyscall();

	execSwitchStack(parent, buffer, sz, loaderStacks[whoami()]);
}

extern "C" void execPartTwo(PID parent, uint64_t buffer, size_t sz) {
	// We're now on a public stack, independent of the syscall that called this
	auto parentpp = getTask(parent);
	auto& parentst = *(parentpp.get());
	size_t npages = (sz + PAGE_SIZE - 1) / PAGE_SIZE;

	// Acquire the loader
	loaderLock.acquire();
	auto loaderpp = getTask(Loader::LOADER_PID);
	loaderpp.acquire();

	// Now, start steal pages from process to loader
	for(size_t i=0; i<npages; ++i) {
		uint64_t phys = parentst.paging.getPhys(buffer + i * PAGE_SIZE);
		parentst.paging.unmap(buffer + i * PAGE_SIZE);
		// phys is now a page not linked anywhere
		if(!Loader::movePage(phys, i))
			bruh(Bruh::LOADER_MOVE_PAGE);
	}

	// That's all for the parent
	setOrigRunning(Loader::LOADER_PID);
	parentpp.release();
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
		// And set the values
		auto& childst = *(childpp.get());
		childst.parent = parent;
		childst.task->jump(entry);
		childpp.release();

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
