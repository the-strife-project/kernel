#include "output.hpp"

size_t row = 0;
size_t col = 0;
char color = DEFAULT_COLOR;

inline void goAhead(size_t n=1) {
	col += n;
	row += col / FB_COLS;
	col %= FB_COLS;
}

inline char* getVideo() {
	return ((char*)VIDEO_BASE) + (FB_COLS*row + col)*2;
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

void getRC(size_t& r, size_t& c) {
	r = row;
	c = col;
}

void resetKernelTerm() {
	row = col = 0;
}
