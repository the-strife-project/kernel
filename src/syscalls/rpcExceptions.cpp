#include <klibc/klibc.hpp>

extern "C" void badPID(PID pid) {
	printf("Bad PID: 0x%x. Should kill.\n", pid);
	hlt();
}
