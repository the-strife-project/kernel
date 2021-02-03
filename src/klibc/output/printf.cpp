#include "output.hpp"
#include "../conversions/conversions.hpp"

void printHex(uint64_t n) {
	char buffer[9] = {0};
	htoa(n, buffer, 8);

	size_t begin = 0;
	while(begin < 6 && buffer[begin] == '0')
		++begin;

	_writes(buffer+begin);
}

void printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	bool acceptingFormat = false;
	for(; *fmt; ++fmt) {
		if(acceptingFormat) {
			switch(*fmt) {
			case 's':
				_writes(va_arg(args, const char*));
				break;
			case 'x':
				printHex(va_arg(args, uint64_t));
				break;
			case 'd':
				break;
			case 'c':
				_writec((char)va_arg(args, int));
				break;
			default:
				_writec(*fmt);
				break;
			}

			acceptingFormat = false;
		} else if(*fmt == '%') {
			acceptingFormat = true;
		} else {
			_writec(*fmt);
		}
	}
}
