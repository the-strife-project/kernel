#ifndef HACKS_HPP
#define HACKS_HPP

#include <common.hpp>

inline bool isPowerOfTwo(size_t x) {
	return (x & (x - 1)) == 0;
}

#endif
