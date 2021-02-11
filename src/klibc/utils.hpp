#ifndef HACKS_HPP
#define HACKS_HPP

#include <common.hpp>

inline bool isPowerOfTwo(size_t x) {
	return (x & (x - 1)) == 0;
}

inline size_t log2(size_t x) {
	size_t ret = 0;
	while(x >>= 1)
		++ret;
	return ret;
}

#endif
