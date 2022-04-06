#include <klibc/klibc.hpp>

extern "C" void catchDE() {
	// TODO: don't crash if userspace
	printf("#DE - Division by zero\n");
	hlt();
	while(true);
}
