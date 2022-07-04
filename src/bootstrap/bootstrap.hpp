#ifndef BOOTSTRAP_HPP
#define BOOTSTRAP_HPP

/*
	The kernel-managed userland bootstrapping sequence starts after the loader
	is set up, and ends as soon as userland is able to load more userland.

	Here, core services will be started, including SATA device drivers
	and filesystems. Things like networking will not be here.
*/

#include <panic/panic.hpp>
#include <tasks/task/task.hpp>

extern PID psnsPID;

namespace Bootstrap {
	PID run(const char* name, size_t moduleID, bool noprint=false);
	void bootstrap();
};

#endif
