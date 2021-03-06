#ifndef PIDS_HPP
#define PIDS_HPP

#include <tasks/scheduler/scheduler.hpp>

Scheduler::SchedulerTask& getTask(PID pid);
PID assignPID(const Scheduler::SchedulerTask&);

#endif
