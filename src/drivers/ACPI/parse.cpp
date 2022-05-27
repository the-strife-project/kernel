#include "ACPI.hpp"
#include <klibc/klibc.hpp>

bool ACPI::atLeastv2;

const char SIGNATURE[] = "RSD PTR ";
const size_t SIGNATURE_LEN = 8;
const uint64_t mapflags = Paging::MapFlag::RO | Paging::MapFlag::NX;

size_t ACPI::entries = 0;
uint32_t* ACPI::rsdtptrs = nullptr;
uint64_t* ACPI::xsdtptrs = nullptr;

void ACPI::parse(stivale2_struct* bootData) {
	// Grab RSDP from stivale2
	union {
		void* tag;
		stivale2_struct_tag_rsdp* rsdp;
	} u;

	u.tag = stivale2_get_tag(bootData, STIVALE2_STRUCT_TAG_RSDP_ID);

	RSDP rsdp;
	rsdp.ptr = u.rsdp->rsdp;

	kpaging.map(rsdp.ptr, rsdp.ptr, sizeof(RSDPDescriptor20), mapflags);

	for(size_t i=0; i<SIGNATURE_LEN; ++i)
		if(rsdp.v1->signature[i] != SIGNATURE[i])
			panic(Panic::RSDP_BAD_SIGNATURE);

	uint8_t checksum = 0;
	for(size_t i=0; i<sizeof(RSDPDescriptor); ++i)
		checksum += rsdp.cptr[i];
	if(checksum) panic(Panic::RSDP_BAD_CHECKSUM);

	atLeastv2 = rsdp.v1->revision;
	if(!atLeastv2) {
		// v1.0
		kpaging.map(rsdp.v1->rsdt, rsdp.v1->rsdt, sizeof(SDTHeader), mapflags);
		SDTHeader* header = (SDTHeader*)(uint64_t)(rsdp.v1->rsdt);
		kpaging.map(rsdp.v1->rsdt, rsdp.v1->rsdt, header->length, mapflags);

		entries = (header->length - sizeof(SDTHeader)) / 4;
		rsdtptrs = (uint32_t*)(rsdp.v1->rsdt + sizeof(SDTHeader));
	} else {
		// v2.0
		for(size_t i=0; i<sizeof(RSDPDescriptor20); ++i)
			checksum += rsdp.cptr[i];
		if(checksum) panic(Panic::RSDP_BAD_CHECKSUM);

		kpaging.map(rsdp.v2->xsdt, rsdp.v2->xsdt, sizeof(SDTHeader), mapflags);
		SDTHeader* header = (SDTHeader*)(rsdp.v2->xsdt);
		kpaging.map(rsdp.v2->xsdt, rsdp.v2->xsdt, header->length, mapflags);

		entries = (header->length - sizeof(SDTHeader)) / 8;
		xsdtptrs = (uint64_t*)(rsdp.v2->xsdt + sizeof(SDTHeader));
	}
}
