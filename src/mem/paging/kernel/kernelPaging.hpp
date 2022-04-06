#include <mem/paging/basic/paging.hpp>
#include <mem/memmap/memmap.hpp>

#define HIGHER_HALF 0xFFFFFFFF80000000

extern "C" Paging kpaging;
void initKernelPaging(MemoryMap);

// Specific memory regions in global page table:
// Last GB (-1): PIDs (tasks/PIDs/PIDs.cpp)
#define GLOBAL_PIDS_REGION (0 - (1ull << 30))
#define GLOBAL_PIDS_SIZE (1 << 30)
// -2 GB: PSNS (IPC/PSNS.cpp)
#define GLOBAL_PSNS_REGION (0 - (2ull << 30))
#define GLOBAL_PSNS_SIZE (1 << 30)
