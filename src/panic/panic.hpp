#ifndef PANIC_HPP
#define PANIC_HPP

#include <asm.hpp>

struct Panic {
	enum {
		TEST,
		NO_MEMORY_MAP,
		EMPTY_MEMORY_MAP,
		PMM_INITIALIZE,
		OUT_OF_MEMORY,
		TOO_MANY_REGIONS,
		NO_LOADER,
		LOADER_NOT_SUS,
		GET_PTE_NO_PAGE,
		ALLOCATOR_INVALID_SIZE,
		ALLOC_INVALID_SIZE,
		PURE_VIRTUAL,
		COULD_NOT_GET_ASLR,
		MLVECTOR_TOO_BIG,
		UNEXPECTED_INTERRUPT,
		DOUBLE_FAULT,
		UNKNOWN_PAGE_FAULT,
		TOO_MANY_CORES,
		GENERAL_PROTECTION_FAULT,
		NO_STDLIB,
		BAD_STDLIB,
		NO_BOOTSTRAP,
		BOOTSTRAP_ELF_TOO_BIG,
		BAD_BOOTSTRAP,
		DISPATCHER_RETURNED,
		RSDP_BAD_SIGNATURE,
		RSDP_BAD_CHECKSUM,
		HELP_ME, // Just needed to get that out
		NO_APIC,
		APIC_ADDRESS_IN_USE,
		APIC_WEIRD_ADDRESS,
		NO_AHCI,
		CRITICAL_PROCESS_DIED,
		NO_MADT,
		NO_IOAPIC,
		ONLY_ONE_IOAPIC,
		NO_GSI_FOR_PIT,
	};
};

void panic(size_t id, bool doNotHalt=false);
[[noreturn]] void hardPanic(size_t id); // Just doNotHalt=false

#endif
