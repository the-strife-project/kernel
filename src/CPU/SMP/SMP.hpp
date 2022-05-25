#ifndef SMP_HPP
#define SMP_HPP

#include <common.hpp>

// First, <tasks/scheduler/scheduler.cpp>'s "running" sets a max to 512
//#define MAX_CORES 512
// Then, <tasks/task/task.hpp>'s "rpcStacks" sets the limit to 256
#define MAX_CORES 256

extern "C" size_t whoami();
extern size_t ncores;

#endif
