#include "job.hpp"

#include "job_monitor.hpp"

void job(int id, int runtimeInMillisec, JobMonitor* jobMon)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(runtimeInMillisec));
  jobMon->removeJob(id);
}
