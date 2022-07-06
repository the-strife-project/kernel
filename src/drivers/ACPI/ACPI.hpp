#ifndef ACPI_HPP
#define ACPI_HPP

#include <common.hpp>
#include <boot/bootdata.hpp>

namespace ACPI {
	// ACPI 1.0
	struct RSDPDescriptor {
		char signature[8];
		uint8_t checksum;
		char OEMID[6];
		uint8_t revision;
		uint32_t rsdt;
	} __attribute__((packed));

	// ACPI >=2.0
	struct RSDPDescriptor20 {
		RSDPDescriptor v1;

		uint32_t length;
		uint64_t xsdt;
		uint8_t checksum;
		uint8_t reserved[3];
	} __attribute__((packed));

	union RSDP {
		uint64_t ptr;
		char* cptr;
		RSDPDescriptor* v1;
		RSDPDescriptor20* v2;
	} __attribute__((packed));

	// Both
	struct SDTHeader {
		char signature[4];
		uint32_t length;
		uint8_t revision;
		uint8_t checksum;
		char OEMID[6];
		char OEMTableID[8];
		uint32_t OEMRevision;
		uint32_t CreatorID;
		uint32_t CreatorRevision;
	} __attribute__((packed));

	extern bool atLeastv2;
	extern size_t entries;
	extern uint32_t* rsdtptrs;
	extern uint64_t* xsdtptrs;

	void parse(stivale2_struct* bootData);
	SDTHeader* get(const char* signature);
};

#endif
