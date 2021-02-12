#include <panic/panic.hpp>

extern "C" void __cxa_pure_virtual() { panic(Panic::PURE_VIRTUAL); }
