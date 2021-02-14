#ifndef ALLOCATORS_HPP
#define ALLOCATORS_HPP

#include "Allocator.hpp"

void initAllocators();
void* alloc(size_t, bool);
void free(void*, size_t, bool);

#endif
