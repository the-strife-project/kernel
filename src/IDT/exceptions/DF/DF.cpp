#include <panic/panic.hpp>

extern "C" void asmDF() {
	panic(Panic::DOUBLE_FAULT);
}
