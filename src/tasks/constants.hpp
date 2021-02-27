#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <common.hpp>

const uint64_t MAX_PROG_PAGES = 1048576;
const uint64_t MAX_HEAP_PAGES = 16777216;
const uint64_t MAX_STACK_PAGES = 2048;

#define PROG_ALIGNMENT PAGE_SIZE
#define HEAP_ALIGNMENT PAGE_SIZE
#define STACK_ALIGNMENT 16

#endif
