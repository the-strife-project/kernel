#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <types>

extern "C" void asmDF();
extern "C" void asmPF();
extern "C" void asmGPF();
extern "C" void asmUD();
extern "C" void asmDE();

[[noreturn]] void exceptionKill(size_t reason);

#endif
