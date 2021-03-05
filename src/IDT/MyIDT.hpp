#ifndef MY_IDT_HPP
#define MY_IDT_HPP

#include "IDT.hpp"

void initIDT();
void IDTsetIST(size_t, uint8_t);
void IDTset(size_t, const IDT::CoolDescriptor&);

#endif
