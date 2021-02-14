#include "output.hpp"

#define VIDEO_BASE 0xFFFFFFFF800B8000
//#define VIDEO_BASE 0xB8000
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

// This will never implement scroll. Shouldn't be used that often.

void _writec(char c) {
	if(c == '\n') {
		++row;
		col = 0;
		return;
	}

	char* video = getVideo();
	*video++ = c;
	*video = color;
	goAhead();
}

void setColor(uint8_t c) { color = c; }
void resetColor() { color = DEFAULT_COLOR; }
