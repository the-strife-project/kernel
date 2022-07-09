#include "stacks.hpp"
#include <mem/VMM/VMM.hpp>

uint64_t* pubStacks;
uint64_t* privStacks;
uint64_t* loaderStacks;

void prepareStacks(size_t CPUs) {
	pubStacks = (uint64_t*)PublicMM::calloc();
	privStacks = (uint64_t*)PublicMM::calloc();
	loaderStacks = (uint64_t*)PhysMM::calloc();

	for(size_t i=0; i<CPUs; ++i) {
		pubStacks[i] = PublicMM::calloc() + PAGE_SIZE;
		privStacks[i] = PhysMM::alloc() + PAGE_SIZE;
		loaderStacks[i] = PhysMM::alloc() + PAGE_SIZE;
	}
}
