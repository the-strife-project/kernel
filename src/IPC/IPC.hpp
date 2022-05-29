#ifndef IPC_HPP
#define IPC_HPP

#include <klibc/klibc.hpp>
#include <types>
#include <tasks/task/task.hpp>

namespace IPC {
	// Public Service Namespace
	void initPSNS();
	bool publish(PID pid, uint64_t name);
	PID resolve(uint64_t name);

	// Shared memory
	std::SMID smMake(Task* me);
	bool smAllow(Task* me, std::SMID smid, PID pid);
	bool smRequest(Task* me, PID myself, PID pid, std::SMID smid);
	uint64_t smMap(Task* me, std::SMID smid);

	// Extra
	uint64_t rpcMoreStacks(PID remote);
};

#endif
