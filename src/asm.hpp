#ifndef ASM_HPP
#define ASM_HPP

#include <common.hpp>

inline void bochs() { asm volatile("xchgw %bx, %bx"); }

inline uint64_t rflags_read() {
	uint64_t ret;
	asm volatile("pushfq\n"
				 "popq %0" : "=a"(ret));
	return ret;
}

inline void rflags_write(uint64_t flags) {
	asm volatile("pushq %0\n"
				 "popfq" : : "a"(flags));
}

inline void cli() { asm volatile("cli"); }
inline void sti() { asm volatile("sti"); }
inline void hlt() { asm volatile("hlt"); }

// I/O
inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

inline uint16_t inw(uint16_t port) {
	uint16_t ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

inline uint32_t inl(uint16_t port) {
	uint32_t ret;
	asm volatile("inl %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

inline void outb(uint16_t port, uint8_t value) {
	asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

inline void outw(uint16_t port, uint16_t value) {
	asm volatile("outw %1, %0" : : "dN"(port), "a"(value));
}

inline void outl(uint16_t port, uint32_t value) {
	asm volatile("outl %1, %0" : : "dN"(port), "a"(value));
}

#endif
