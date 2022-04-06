#ifndef IPC_HPP
#define IPC_HPP

#include <klibc/klibc.hpp>

namespace IPC {
	// Public Service Namespace
	void initPSNS();
	bool publish(PID pid, uint64_t name);
	PID resolve(uint64_t name);
};

#endif
