#ifndef STACKS_HPP
#define STACKS_HPP

#include <common.hpp>

/*
	It is necessary to have two stacks (pages) for each CPU.
	When a syscall occurs, the privStack is loaded. On an interrupt,
	a pubStack is set, because it has to be mapped to the page table of the process.
	Then, once everything has been pushed to the public stack, it switches to the
	private one.
*/

extern "C" uint64_t* pubStacks;	// Public stacks for each TSS
extern uint64_t* privStacks;	// Private stacks for everything
extern "C" uint64_t* loaderStacks; // Private stacks when switching to loader

void prepareStacks(size_t CPUs);

#endif
