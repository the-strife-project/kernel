#ifndef MY_IDT_HPP
#define MY_IDT_HPP

#include "IDT.hpp"

void initIDT();
void IDTset(size_t, const IDT::CoolDescriptor&);

#endif
