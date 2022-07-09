#ifndef MLFQ_HPP
#define MLFQ_HPP

#include "RR.hpp"

// Multilevel Feedback Queue

class MLFQ {
private:
	size_t nprio;

	// The RoundRobin class is well protected by locks
	RoundRobin* rr = nullptr;
	size_t approxSize = 0;

public:
	MLFQ(size_t nprio_) {
		nprio = nprio_;
		rr = (RoundRobin*)PhysMM::calloc();
		for(size_t i=0; i<nprio; ++i)
			rr[i] = RoundRobin();
	}

	// Approximate size. It's not guaranteed to be correct, as it doesn't lock.
	// There's no actual need to know the number of threads in runnable state,
	//   so this is a neat approximation. It's unlikely, but it might go nuts (~0).
	inline size_t getApproxSize() const { return approxSize; }

	void add(PID pid);

	inline PID pop() {
		PID ret;
		for(size_t i=0; i<nprio; ++i) {
			if((ret = rr[i].pop())) {
				--approxSize;
				return ret;
			}
		}
		return 0;
	}
};

#endif
