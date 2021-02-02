#include "output.hpp"

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
