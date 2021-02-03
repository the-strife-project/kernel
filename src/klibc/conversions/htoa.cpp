#include "conversions.hpp"

const char CHARSET[] = "0123456789ABCDEF";

void htoa(size_t n, char* buffer, size_t sz) {
	while(sz--) {
		buffer[sz] = CHARSET[n & 0x0F];
		n >>= 4;
	}
}
