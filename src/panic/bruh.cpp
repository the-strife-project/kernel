#include "bruh.hpp"
#include <klibc/klibc.hpp>

const char* const msgs[] = {
	"Syscall from PID 0",
	"getTask() on empty SchedulerTask*",
	"On bootstrap, acquisition got null",
	"Protected pointer get() or isNull() while not acquired",
	"smAllow reached an impossible flow",
	"Loader::movePage says page already mapped!",
	"PhysMM::free() with page from inexisting region",
};

[[noreturn]] void bruh(size_t id) {
	setColor(0x4F);
	printf("#BRUH\n\n%s", msgs[id]);
	hlt(); while(true);
}
