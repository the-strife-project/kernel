#ifndef SPINLOCK_HPP
#define SPINLOCK_HPP

#include <common.hpp>

class Spinlock {
private:
	volatile uint32_t lock = 0;

public:
	inline void acquire() {
		asm volatile("0:\n"
					 "lock btsl $0, %0\n"
					 "jnc 2f\n"

					 "1:\n"
					 "pause\n"
					 "btl $1, %0\n"
					 "jnc 1b\n"
					 "jmp 0b\n"

					 "2:"
					 : "+m"(lock)
					 :: "memory", "cc");
	}

	inline void release() { lock = 0; }
};

#endif
