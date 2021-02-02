#include "output.hpp"

#define VIDEO_BASE 0xB8000
#define COLS 80
#define DEFAULT_COLOR 0x0A

size_t row = 0;
size_t col = 0;
char color = DEFAULT_COLOR;

inline void goAhead(size_t n=1) {
	col += n;
	row += col / COLS;
	col %= COLS;
}

inline char* getVideo() {
	return ((char*)VIDEO_BASE) + (COLS*row + col)*2;
}

void _writec(char c) {
	char* video = getVideo();
	*video++ = c;
	*video = color;
	goAhead();
}

void _writes(const char* str) {
	char* video = getVideo();
	size_t count = 0;
	for(;str[count];++count) {
		*video++ = str[count];
		*video++ = color;
	}

	goAhead(count);
}
