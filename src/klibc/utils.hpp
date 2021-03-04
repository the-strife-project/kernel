#ifndef UTILS_HPP
#define UTILS_HPP

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

template<typename T> T&& move(T& x) { return static_cast<T&&>(x); }

#endif
