#include "output.hpp"
#include "../conversions/conversions.hpp"
#include "../spinlock.hpp"

void printHex(uint64_t n, size_t fixedSize=0) {
	char buffer[17] = {0};
	htoa(n, buffer, 16);

	size_t begin = 0;
	if(!fixedSize) {
		while(begin < 15 && buffer[begin] == '0')
			++begin;
	} else {
		begin = 16 - fixedSize;
	}

	_writes(buffer+begin);
}

void dump(uint64_t addr, size_t sz) {
	while(sz--) {
		size_t c = *(char*)(addr++);
		c &= 0xFF;
		printHex(c, 2);
		_writec(' ');
	}
}

static Spinlock lock;

void printf(const char* fmt, ...) {
	//lock.acquire();
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
			case 'D':
				// Dump
				{
					uint64_t addr = va_arg(args, uint64_t);
					size_t sz = va_arg(args, size_t);
					dump(addr, sz);
				}
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
	//lock.release();
}
