#include "syscalls.hpp"
#include <asm.hpp>
#include <klibc/klibc.hpp>
#include <tasks/scheduler/scheduler.hpp>
#include <CPU/SMP/SMP.hpp>
#include <mem/stacks/stacks.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <IPC/IPC.hpp>
#include <kkill>
#include <panic/bruh.hpp>
#include <bootstrap/bootstrap.hpp>

// These kill current task, which is done lock-free
inline static void onlyLoader(PID pid, Scheduler::SchedulerTask& stask) {
	if(pid != Loader::LOADER_PID)
		stask.kill(pid, std::kkill::LOADER_SYSCALL);
}

extern "C" void RPCerr(Scheduler::SchedulerTask* task) {
	task->kill(getRunningAs(), std::kkill::RPC_BAD_PID);
}

inline void onlyKernel(PID pid, Scheduler::SchedulerTask& stask) {
	// Check if syscall was called by the kernel
	// That is, RIP must have been at higher half
	// RIP is one of the fields at the SavedState, as RCX
	SavedState* ss = savedState[whoami()];
	SavedState copy;
	pmemcpy(&copy, stask.paging, ss, sizeof(SavedState));
	if((copy.regs.rcx & (1ull << 63)) == 0)
		stask.kill(pid, std::kkill::KERNEL_SYSCALL);
}

// Just add arguments as they needed. va_list doesn't work here.
extern "C" uint64_t syscallHandler(size_t op, size_t arg1, size_t arg2,
								   size_t arg3) {
	uint64_t ret = 0; // Return value of this syscall
	PID pid = getRunningAs();
	if(!pid)
		bruh(Bruh::SYSCALL_FROM_PID_0);

	// Got a syscall. Task is not executing anymore.
	// Its PCB is now free to be modified. However, it will be accesed
	//   right away, so no need to release the lock.
	auto pp = getTask(pid);
	pp.acquire(); // No one does anything with PID
	if(pp.isNull()) {
		// damn, got killed
		pp.release();
		schedule();
	}

	Scheduler::SchedulerTask& stask = *(pp.get());

	bool goBack = true;

	switch(op) {
	// --- GENERIC ---
	case std::Syscalls::EXIT:
		stask.exit(arg1);
		goBack = false;
		break;
	case std::Syscalls::MORE_HEAP:
		ret = stask.task->moreHeap(arg1);
		break;
	case std::Syscalls::MMAP:
		ret = stask.task->mmap(arg1, arg2);
		break;

	// --- LOADER ONLY ---
	case std::Syscalls::BACK_FROM_LOADER:
		onlyLoader(pid, stask);
		Loader::imBack(arg1, arg2, arg3);
		goBack = false; // That's it for now
		break;
	case std::Syscalls::MAKE_PROCESS:
		onlyLoader(pid, stask);
		ret = Loader::makeProcess();
		break;
	case std::Syscalls::ASLR_GET: {
		onlyLoader(pid, stask);
		auto ppaslr = getTask(arg1);
		ppaslr.acquire();
		if(ppaslr.isNull())
			ret = ~0; // And hope it crashes
		else
			ret = ppaslr.get()->task->getASLR().getFromID(arg2, arg3);
		ppaslr.release();
		} break;
	case std::Syscalls::MAP_IN:
		onlyLoader(pid, stask);
		ret = Loader::mapIn(arg1, arg2, arg3);
		break;

	// --- IPC ---
	case std::Syscalls::FIND_PSNS:
		ret = psnsPID;
		break;
	case std::Syscalls::HALT:
		stask.task->saveStateSyscall();
		stask.task->freeStack();
		goBack = false;
		break;
	case std::Syscalls::ENABLE_RPC:
		stask.task->setRPCentry(arg1);
		break;
	case std::Syscalls::RPC_MORE_STACKS:
		onlyKernel(pid, stask);
		ret = IPC::rpcMoreStacks(arg1);
		break;

	// --- SHARED MEMORY ---
	case std::Syscalls::SM_MAKE:
		ret = IPC::smMake(stask.task);
		break;
	case std::Syscalls::SM_ALLOW:
		ret = IPC::smAllow(stask.task, arg1, arg2);
		break;
	case std::Syscalls::SM_REQUEST:
		ret = IPC::smRequest(stask.task, pid, arg1, arg2);
		break;
	case std::Syscalls::SM_MAP:
		ret = IPC::smMap(stask.task, arg1);
		break;

	// --- SPECIAL PERMISSIONS ---
	case std::Syscalls::ALLOW_IO:
		// A check would go here (running as system?)
		ret = getIO(pid, stask.task);
		break;
	case std::Syscalls::ALLOW_PHYS:
		// Check here!
		stask.task->allowPhys();
		ret = 1;
		break;
	case std::Syscalls::GET_PHYS:
		if(!stask.task->isPhysAllowed())
			stask.kill(pid, std::kkill::PHYS_NOT_ALLOWED);

		ret = stask.task->getPaging().getPhys(arg1);
		break;
	case std::Syscalls::MAP_PHYS:
		if(!stask.task->isPhysAllowed())
			stask.kill(pid, std::kkill::PHYS_NOT_ALLOWED);

		ret = stask.task->mapPhys(arg1, arg2, arg3);
		break;

	// --- TASK-RELATED ---
	case std::Syscalls::EXEC:
		exec(pid, arg1, arg2);
		// That might return in case of bad buffer
		ret = NULL_PID;
		break;
	case std::Syscalls::GET_LAST_LOADER_ERROR:
		ret = stask.lastLoaderError;
		break;
	case std::Syscalls::GET_KILL_REASON:
		ret = std::kkill::OK;
		for(auto const& x : stask.children) {
			if(x.pid == arg1) {
				ret = x.kr;
				break;
			}
		}
		break;
	case std::Syscalls::GET_EXIT_VALUE:
		ret = ~0ull;
		for(auto const& x : stask.children) {
			if(x.pid == arg1) {
				ret = x.ret;
				break;
			}
		}
		break;
	case std::Syscalls::WAIT:
		ret = 0;
		wait(pid, arg1);
		break;

	default:
		stask.kill(pid, std::kkill::UNKNOWN_SYSCALL);
		goBack = false;
	}

	pp.release();
	if(!goBack)
		schedule();

	return ret;
}
