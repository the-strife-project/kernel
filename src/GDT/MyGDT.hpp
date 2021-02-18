#ifndef MYGDT_HPP
#define MYGDT_HPP

#include <common.hpp>

#define SEGMENT_SELECTOR_KCODE 0x08
#define SEGMENT_SELECTOR_KDATA 0x10
#define SEGMENT_SELECTOR_UDATA 0x18
#define SEGMENT_SELECTOR_UCODE 0x20

#define USER_PL 3

void initGDT();
uint16_t newTSSsegment(uint64_t addr);

#endif
