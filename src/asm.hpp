#ifndef ASM_HPP
#define ASM_HPP

#include <common.hpp>

#define N_CALLEE_SAVED 6

inline uint64_t rflags_read() {
	uint64_t ret;
	asm volatile("pushfq\n"
				 "popq %0" : "=a"(ret));
	return ret;
}

inline void rflags_write(uint64_t flags) {
	asm volatile("pushq %0\n"
				 "popfq" :: "a"(flags));
}

inline void cli() { asm volatile("cli"); }
inline void sti() { asm volatile("sti"); }
inline void hlt() { asm volatile("hlt"); }

inline uint64_t getCR3() {
	uint64_t ret = ~0;
	asm volatile("mov %%cr3, %0" : "=a"(ret));
	return ret;
}
inline void setCR3(uint64_t v) {
	asm volatile("mov %0, %%cr3" :: "a"(v));
}

inline uint64_t getCR4() {
	uint64_t ret = ~0;
	asm volatile("mov %%cr4, %0" : "=a"(ret));
	return ret;
}
inline void setCR4(uint64_t v) {
	asm volatile("mov %0, %%cr4" :: "a"(v));
}

inline uint64_t getCR2() {
	uint64_t ret;
	asm volatile("mov %%cr2, %0" : "=r"(ret));
	return ret;
}

inline void invlpg(uint64_t page) {
	asm volatile("invlpg (%0)" :: "r"(page));
}

inline void sysret() {
	asm volatile("sysretq");
}

// TODO possible strict aliasing violation
inline uint32_t* higherHalf_uint64(uint64_t* x) { return ((uint32_t*)x) + 1; }

typedef uint32_t MSR;
inline uint64_t rdmsr(MSR addr) {
	uint64_t ret;
	asm volatile("rdmsr" : "=d"(*higherHalf_uint64(&ret)), "=a"(ret) : "c"(addr));
	return ret;
}
inline void wrmsr(MSR addr, uint64_t contents) {
	asm volatile("wrmsr" :: "c"(addr), "d"(*higherHalf_uint64(&contents)), "a"(contents));
}

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
	asm volatile("outb %1, %0" :: "dN"(port), "a"(value));
}

inline void outw(uint16_t port, uint16_t value) {
	asm volatile("outw %1, %0" :: "dN"(port), "a"(value));
}

inline void outl(uint16_t port, uint32_t value) {
	asm volatile("outl %1, %0" :: "dN"(port), "a"(value));
}

#endif
