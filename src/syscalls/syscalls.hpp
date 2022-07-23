#ifndef SYSCALLS_HPP
#define SYSCALLS_HPP

#define MSR_EFER  0xC0000080
#define MSR_STAR  0xC0000081
#define MSR_LSTAR 0xC0000082

#define EFER_SYSCALL_ENABLE 0

#define STAR_SYSCALL_SELECTOR 32
#define STAR_SYSRET_SELECTOR  48

#include <stdarg.h>
#include <common.hpp>
#include <syscalls> // Header from stdlib, has "std::Syscalls" enum
#include <tasks/task/task.hpp>
#include <tasks/scheduler/scheduler.hpp>

void enableSyscalls();

// Specifics
size_t getIO(std::PID pid, Task* task);
void exec(PID parent, uint64_t buffer, size_t sz, uint64_t runtime, size_t rtsz);
extern "C" uint64_t* loaderStacks;
void wait(PID parent, PID child);

void lockCurrent();
bool wake(PID);
void taskInfo(PID me, Task*, PID pid, uint64_t);
bool switchUser(PID pid, size_t uid);

bool sysCSPRNG(Task*, uint64_t virt, size_t sz);

#endif
