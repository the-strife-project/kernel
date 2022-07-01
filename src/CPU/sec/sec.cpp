#include "sec.hpp"
#include <asm.hpp>

bool smapSupported = false;

void Security::SMAPon() {
	if(smapSupported)
		asm volatile("clac" ::: "cc");
}

void Security::SMAPoff() {
	if(smapSupported)
		asm volatile("stac" ::: "cc");
}

inline uint64_t cpuid7b() {
	uint64_t rbx;
	asm volatile("cpuid"
				 : "=b" (rbx)
				 : "a" (7),
				   "c" (0)
				 : "cc", "memory");
	return rbx;
}

void Security::enableSMEP() {
	if(cpuid7b() & 1 << CPUID_SMEP)
		setCR4(getCR4() | 1 << CPU_CR4_SMEP_BIT);
}

void Security::enableSMAP() {
	if(cpuid7b() & 1 << CPUID_SMAP) {
		setCR4(getCR4() | 1 << CPU_CR4_SMAP_BIT);
		smapSupported = true;
		SMAPon();
	}
}
