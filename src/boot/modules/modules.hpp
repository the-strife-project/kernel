#ifndef STIVALE2_MODULES_HPP
#define STIVALE2_MODULES_HPP

#include <boot/bootdata.hpp>

namespace BootModules {
	extern uint64_t begins[];
	extern uint64_t sizes[];

	enum {
		MODULE_ID_LOADER,
		MODULE_ID_STDLIB,
		MODULE_ID_TERM,
		NMODULES
	};

	void save(stivale2_struct*);
}

#endif
