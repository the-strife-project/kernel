#include "APIC.hpp"
#include "../ACPI/ACPI.hpp"
#include <klibc/klibc.hpp>

uint8_t APIC::IOAPIC::id = 0;
uint32_t APIC::IOAPIC::address = 0;
uint32_t APIC::IOAPIC::gsiBase = 0;
List<APIC::IOAPIC::MADT_ISO> APIC::IOAPIC::isos;

void APIC::IOAPIC::init() {
	// Where's the IOAPIC? MADT, the ACPI table, knows
	ACPI::SDTHeader* sdt = ACPI::get("APIC");
	if(!sdt)
		panic(Panic::NO_MADT);

	auto length = sdt->length;

	uint64_t raw = (uint64_t)sdt;
	size_t off = 0x2C;
	while(off < length) {
		uint8_t entryType = *(uint8_t*)(raw + off);
		++off;
		uint8_t recordLength = *(uint8_t*)(raw + off);
		++off;

		switch(entryType) {
		case 1: {
			// Found it!
			if(APIC::IOAPIC::address)
				panic(Panic::ONLY_ONE_IOAPIC);

			auto* ioapic = (MADT_IOAPIC*)(raw + off);
			APIC::IOAPIC::id = ioapic->id;
			APIC::IOAPIC::address = ioapic->address;
			APIC::IOAPIC::gsiBase = ioapic->gsiBase;
			break; }
		case 2: {
			auto* iso = (MADT_ISO*)(raw + off);
			APIC::IOAPIC::isos.push_back(*iso);
			break; }
		}

		off += recordLength - 2;
	}

	if(!APIC::IOAPIC::address)
		panic(Panic::NO_IOAPIC);

	// Simple map of one page
	size_t mf = Paging::MapFlag::PCD | Paging::MapFlag::NX;
	kpaging.map(APIC::IOAPIC::address, APIC::IOAPIC::address, PAGE_SIZE, mf);
}

uint32_t APIC::IOAPIC::readReg(uint8_t offset) {
	*(uint32_t*)(address + IOREGSEL) = offset;
	return *(uint32_t*)(address + IOWIN);
}

void APIC::IOAPIC::writeReg(uint8_t offset, uint32_t val) {
	*(uint32_t*)(address + IOREGSEL) = offset;
	*(uint32_t*)(address + IOWIN) = val;
}

size_t APIC::IOAPIC::readRedirection(APIC::IOAPIC::GSI gsi) {
	uint32_t reg = ((gsi - gsiBase) * 2) + 16;
	return readReg(reg) | ((size_t)readReg(reg + 1) << 32);
}

void APIC::IOAPIC::writeRedirection(APIC::IOAPIC::GSI gsi, uint64_t entry) {
	uint32_t reg = ((gsi - gsiBase) * 2) + 16;
	writeReg(reg, entry); // Low 32 bits
	writeReg(reg+1, entry >> 32); // High 32 bits
}

void APIC::IOAPIC::setIRQRedirection(uint32_t lapicID, uint8_t vec, APIC::IOAPIC::GSI gsi, size_t irqflags) {
	// Starts masked when set
	uint64_t flags = MASKED;

	if(irqflags & FLAG_EDGE_TRIGGERED)
		flags |= INTPOL;
	else if(irqflags & FLAG_LEVEL_TRIGGERED)
		flags |= TRIGGER_MODE;

	uint64_t entry = vec | flags | (((uint64_t)lapicID) << LID_SHIFT);
	writeRedirection(gsi, entry);
}

void APIC::IOAPIC::mask(APIC::IOAPIC::GSI gsi) {
	uint64_t entry = readRedirection(gsi);
	entry |= MASKED;
	writeRedirection(gsi, entry);
}

void APIC::IOAPIC::unmask(APIC::IOAPIC::GSI gsi) {
	uint64_t entry = readRedirection(gsi);
	entry &= ~MASKED;
	writeRedirection(gsi, entry);
}
