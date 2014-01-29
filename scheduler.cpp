#include "scheduler.hpp"

#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "resource_monitor.hpp"
#include "job_monitor.hpp"

//(resource needed, time of addition)
typedef std::pair<int, std::chrono::system_clock::time_point> WaitingJob;
typedef std::vector<WaitingJob> JobQueue;

struct OldestWithMaxResourceNeed
{
  OldestWithMaxResourceNeed(int resMax) : mResMax(resMax) {}
  bool operator()(const WaitingJob& lhs, const WaitingJob& rhs)
  {
    if (lhs.first > mResMax) return true;
    if (rhs.first > mResMax) return false;
    return lhs.second < rhs.second;
  }
  int mResMax;
};

int getJobMillisecs()
{
  return std::rand() % 30 * 100 + 100;
}

void scheduler(const char* jobsFile, ResourceMonitor* resMonPtr)
{
  ResourceMonitor& resMon(*resMonPtr);
  JobMonitor jobMonitor(resMon);

  JobQueue jobQ;

  std::ifstream in(jobsFile);
  do
  {
    int jobRes;
    if (in >> jobRes)
    {
      int nodeId = resMon.findBigEnough(jobRes);
      if (-1 != nodeId)
      {
        JobMonitor::ResourceDescriptor desc{ { nodeId, jobRes } };
        jobMonitor.addJob(getJobMillisecs(), desc);
      }
      else
      {
        jobQ.push_back({ jobRes, std::chrono::system_clock::now() });
      }
    }

    int nextJobArrivesIn(std::rand() % 10 + 2);
    while (0 < nextJobArrivesIn--)
    {
      //find oldest waiting that fits
      int freeRes(resMon.allFree());
      if (0 < freeRes)
      {
        auto iter(std::max_element(jobQ.begin(), jobQ.end(), OldestWithMaxResourceNeed(freeRes)));
        if (iter != jobQ.end() && iter->first <= freeRes)
        {
          JobMonitor::ResourceDescriptor desc;

          jobRes = iter->first;
          int remainingRes(jobRes);
          while (remainingRes > 0)
          {
            int nodeId(resMon.findBigEnough(jobRes));
            if (-1 != nodeId)
            {
              desc.push_back({ nodeId, jobRes });
              remainingRes -= jobRes;
              if (remainingRes < jobRes)
              {
                jobRes = remainingRes;
              }
            }
            else
              --jobRes;
          }
          jobMonitor.addJob(getJobMillisecs(), desc);
          jobQ.erase(iter);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  } while (!jobQ.empty() || !jobMonitor.mJobRecords.empty());
}
