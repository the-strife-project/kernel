#include "ACPI.hpp"
#include <klibc/klibc.hpp>

const uint64_t mapflags = Paging::MapFlag::RO | Paging::MapFlag::NX;

ACPI::SDTHeader* ACPI::get(const char* signature) {
	for(size_t i=0; i<entries; ++i) {
		SDTHeader* header;
		if(!atLeastv2)
			header = (SDTHeader*)(uint64_t)(rsdtptrs[i]);
		else
			header = (SDTHeader*)xsdtptrs[i];

		uint64_t ptr = (uint64_t)header;
		kpaging.map(ptr, ptr, sizeof(SDTHeader), mapflags);
		kpaging.map(ptr, ptr, header->length, mapflags);

		bool match = true;
		for(size_t j=0; j<4; ++j) {
			if(header->signature[j] != signature[j]) {
				match = false;
				break;
			}
		}

		// TODO: Checksum

		if(match)
			return header;
	}

	return nullptr;
}
