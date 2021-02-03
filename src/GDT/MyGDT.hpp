#ifndef MYGDT_HPP
#define MYGDT_HPP

#define GDT_N_ENTRIES 3

#define SEGMENT_SELECTOR_KDATA 0x08
#define SEGMENT_SELECTOR_KCODE 0x10

void initGDT();

#endif
