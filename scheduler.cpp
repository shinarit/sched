#include "scheduler.hpp"

#include <thread>
#include <chrono>

#include "resource_monitor.hpp"
#include "job_monitor.hpp"

void scheduler(const char* jobsFile, ResourceMonitor* resMonPtr)
{
  JobMonitor jobMonitor(*resMonPtr);

}
