#ifndef REGISTERS_HPP
#define REGISTERS_HPP

// Registers pushed onto the stack when an interrupt happens
struct GeneralRegisters {
	uint64_t rax = 0;
	uint64_t rbx = 0;
	uint64_t rcx = 0;
	uint64_t rdx = 0;
	uint64_t rsi = 0;
	uint64_t rdi = 0;
	uint64_t rbp = 0;
	uint64_t r8 = 0;
	uint64_t r9 = 0;
	uint64_t r10 = 0;
	uint64_t r11 = 0;
	uint64_t r12 = 0;
	uint64_t r13 = 0;
	uint64_t r14 = 0;
	uint64_t r15 = 0;
} __attribute__((packed));	// 120 bytes

struct RFLAGS {
	enum {
		CF,
		MBO,
		PF,
		MBZ0,
		AF,
		MBZ1,
		ZF,
		SF,
		TF,
		IF,
		DF,
		OF,
		IOPL0,
		IOPL1,
		NT,
		MBZ2,
		RF,
		VM,
		AC,
		VIF,
		VIP,
		ID
	};
};

#define BASIC_RFLAGS ((1 << RFLAGS::MBO) | (1 << RFLAGS::IF))

// Saved state from a process on syscall (in the stack, go read asmhandler)
struct SavedState {
	uint64_t segment = 0;
	uint64_t rflags = BASIC_RFLAGS;
	GeneralRegisters regs;
} __attribute__((packed));

// One SavedState pointer per CPU
extern "C" SavedState* savedState[PAGE_SIZE / sizeof(SavedState*)];

#endif
