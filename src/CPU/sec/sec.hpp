#ifndef SEC_HPP
#define SEC_HPP

#include <common.hpp>

extern "C" bool smapSupported;

namespace Security {
	const size_t CPUID_SMEP = 7;
	const size_t CPUID_SMAP = 20;

	const size_t CPU_CR4_SMEP_BIT = 20;
	const size_t CPU_CR4_SMAP_BIT = 21;

	void SMAPon();
	void SMAPoff();

	void enableSMEP();
	void enableSMAP();
};

#endif
