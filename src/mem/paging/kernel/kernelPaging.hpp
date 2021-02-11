#include <mem/paging/basic/paging.hpp>
#include <mem/memmap/memmap.hpp>

#define HIGHER_HALF 0xFFFFFFFF80000000

extern Paging kpaging;
void initKernelPaging(MemoryMap);
