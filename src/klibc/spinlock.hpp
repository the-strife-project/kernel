#ifndef SPINLOCK_HPP
#define SPINLOCK_HPP

#include <common.hpp>
#include <panic/bruh.hpp>

// This is a "test and test-and-set" lock around LSB
class Spinlock {
private:
	volatile uint64_t lock = 0;

public:
	inline void acquire() {
		asm volatile("0:\n"
					 "lock btsq $0, %0\n"
					 "jnc 2f\n"

					 "1:\n"
					 "pause\n"
					 "testq $1, %0\n"
					 "jnc 1b\n"
					 "jmp 0b\n"

					 "2:"
					 : "+m"(lock)
					 :: "memory", "cc");
	}

	inline void release() { lock &= ~1; }
};

// Protected pointer
// Construct as: ProtPtr(&ptr)
// Requires ptr to be in global memory
// Used in <tasks/PIDs/PIDs.hpp>
// This is pretty much a mutex semaphore
template<typename T>
class ProtPtr {
private:
	union {
		T** in;
		Spinlock* lock;
		T** out;
	} u;

public:
	ProtPtr() = default;
	ProtPtr(T** ptr) { u.in = ptr; }

	inline void acquire() { u.lock->acquire(); }
	inline void release() { u.lock->release(); }

	void assertAcquired() const {
		uint64_t ptr = (uint64_t)*(u.out);
		if((ptr & 1) == 0)
			bruh(Bruh::PROTPTR_NOT_ACQUIRED);
	}

	// Object access
	T* get() const {
		// As the lock has been acquired, LSB=0
		assertAcquired();
		uint64_t ret = (uint64_t)*(u.out);
		ret &= ~1;
		return (T*)ret;
	}

	bool isNull() const {
		assertAcquired();
		uint64_t ret = (uint64_t)*(u.out);
		return ret == 1; // Acquired, so not 0, but 1
	}

	void setNull() {
		assertAcquired();
		*(u.out) = (T*)1; // Acquired, so not 0, but 1
	}
};

#endif
