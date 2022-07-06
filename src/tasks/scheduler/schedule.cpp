#include "scheduler.hpp"
#include <CPU/SMP/SMP.hpp>
#include <tasks/PIDs/PIDs.hpp>
#include <drivers/APIC/APIC.hpp>

// TODO: Make this multicore
uint64_t savedKernelState_rsp = 0;
uint64_t savedKernelState[N_CALLEE_SAVED]; // callee-saved only

[[noreturn]] void schedule() {
	// Is boot finished?
	if(savedKernelState_rsp)
		asmRestoreKernel();

	// Any process ready?
	PID pid = sched.get();
	if(!pid) {
		// Nothing to do
		printf("\nThere's nothing to do.\n");
		hlt();
		while(true);
	}

	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		// Just take it out and go for another
		pp.release();
		schedule();
	}

	// Got it
	Task* task = pp.get()->task;
	setOrigRunning(pid);
	setRunningAs(task->getAs());
	pp.release();

	// Start quantum
	APIC::startQuantum();

	task->dispatch();
	// Never returns

	hardPanic(Panic::DISPATCHER_RETURNED);
}
