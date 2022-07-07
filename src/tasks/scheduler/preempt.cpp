#include <tasks/scheduler/scheduler.hpp>
#include <drivers/APIC/APIC.hpp>
#include <tasks/PIDs/PIDs.hpp>

extern "C" void preempt(SavedState* state, uint64_t rip, uint64_t rsp) {
	kpaging.load();

	if(rip & (1ull << 63)) {
		// Kernel preemption
		APIC::EOI();
		// Try again in 1ms
		APIC::anotherChance();
		return;
	}

	PID pid = origRunning[whoami()];
	auto pp = getTask(pid);
	pp.acquire();
	if(pp.isNull()) {
		// How? Doesn't matter...
		pp.release();
		schedule();
	}

	// Set saved state
	Task* task = pp.get()->task;
	memcpy(&(task->getState()), state, sizeof(SavedState));
	task->jump(rip);
	task->setStack(rsp);

	// Set "as" value to runningAs
	task->setAs(runningAs[whoami()]);

	// Consumed all quantum, let the scheduler know
	pp.get()->ioBurst = false;
	pp.release();

	sched.add(pid);

	APIC::EOI();
	schedule();
}
