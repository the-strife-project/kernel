#include "output.hpp"
#include <klibc/memory/memory.hpp>

// This driver starts on its own, with boot cursor (row and column).
// When term is running, it syncs the cursor with it.

size_t bootRow = 0;
size_t bootCol = 0;
char color = DEFAULT_COLOR;

size_t* row = &bootRow;
size_t* col = &bootCol;

static uint64_t* kcursor = nullptr;

void nowSyncWithTerm(size_t* kcursor_) {
	kcursor = kcursor_;
	kcursor[0] = bootRow;
	kcursor[1] = bootCol;
	row = &kcursor[0];
	col = &kcursor[1];
}

// This driver is a copy-paste from term's w/o the comments. Go read that one.

inline static char* getChar(size_t r, size_t c) {
	return ((char*)VIDEO_BASE) + (FB_COLS * r + c)*2;
}

inline static char* getVideo() { return getChar(*row, *col); }

inline static void clearRow(size_t r) {
	char* buffer = getChar(r, 0);
	memset(buffer, 0, FB_COLS*2);
}

inline static void scroll() {
	void* from = (void*)(VIDEO_BASE + FB_BYTES_PER_ROW);
	memmove((void*)VIDEO_BASE, from, (FB_ROWS-1)*FB_BYTES_PER_ROW);
	--*row;
	clearRow(FB_ROWS-1);
}

inline static void goAhead() {
	++*col;
	if(*col >= FB_COLS) {
		++*row;
		*col = 0;
		if(*row >= FB_ROWS)
			scroll();
	}
}

void _writec(char c) {
	if(c == '\n') {
		++*row;
		*col = 0;
		if(*row >= FB_ROWS) scroll();
		return;
	}

	char* video = getVideo();
	*video++ = c;
	*video = color;
	goAhead();
}

void setColor(uint8_t c) { color = c; }
void resetColor() { color = DEFAULT_COLOR; }
