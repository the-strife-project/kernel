#ifndef ALLOCATORS_HPP
#define ALLOCATORS_HPP

#include "Allocator.hpp"

//template<size_t _SIZE> using PubAllocator = Allocator<_SIZE, VMM::Public::alloc, VMM::Public::free>;
template<size_t _SIZE> using PrivAllocator = Allocator<_SIZE, VMM::Private::alloc, VMM::Private::free>;

void* alloc(size_t, bool);
void free(void*, size_t, bool);

#endif
