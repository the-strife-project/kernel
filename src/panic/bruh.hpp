#ifndef BRUH_HPP
#define BRUH_HPP

// "bruh()" is like "panic()" except only for bugs
// A bruh cannot be caused by the user. It's for impossible kernel situations

#include <asm.hpp>

struct Bruh {
	enum {
		HT64_NULL_KEY,
		HT64_NO_SPACE,
		HT64_FULL_ON_LOOKUP,
		SYSCALL_FROM_PID_0,
		GET_TASK_EMPTY,
		BOOTSTRAP_NULL,
		PROTPTR_NOT_ACQUIRED,
		SM_ALLOW_IMPOSSIBLE
	};
};

[[noreturn]] void bruh(size_t id);

#endif
