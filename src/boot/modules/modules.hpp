#ifndef STIVALE2_MODULES_HPP
#define STIVALE2_MODULES_HPP

#include <boot/bootdata.hpp>

namespace stivale2Modules {
	extern uint64_t loader_beg, loader_end;

	void save(stivale2_struct*);
}

#endif
