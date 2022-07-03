#ifndef STIVALE2_MODULES_HPP
#define STIVALE2_MODULES_HPP

#include <boot/bootdata.hpp>

namespace BootModules {
	extern uint64_t begins[];
	extern uint64_t sizes[];

	enum {
		MODULE_ID_LOADER,
		MODULE_ID_STDLIB,
		MODULE_ID_PSNS,
		MODULE_ID_TERM,
		MODULE_ID_PCI,
		MODULE_ID_AHCI,
		MODULE_ID_RAMBLOCK,
		MODULE_ID_BLOCK,
		MODULE_ID_ISO9660,
		MODULE_ID_STRIFEFS,
		MODULE_ID_VFS,
		NMODULES
	};

	void save(stivale2_struct*);
}

#endif
