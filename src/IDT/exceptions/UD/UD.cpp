#include <klibc/klibc.hpp>

extern "C" void catchUD(size_t err) {
	printf("#UD\n");
	printf("0x%x\n", err);
	hlt();
	while(true);
}
