#include "bruh.hpp"
#include <klibc/klibc.hpp>

const char* const msgs[] = {
	"HT64: NULL KEY",
	"HT64: NO SPACE",
	"HT64: FULL ON LOOKUP",
	"Syscall from PID 0"
};

[[noreturn]] void bruh(size_t id) {
	setColor(0x4F);
	printf("#BRUH\n\n%s", msgs[id]);
	hlt(); while(true);
}