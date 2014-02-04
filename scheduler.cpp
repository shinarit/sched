#include "scheduler.hpp"

#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <mutex>

#include "resource_monitor.hpp"
#include "job_monitor.hpp"

//(resource needed, time of addition)
typedef std::pair<int, std::chrono::system_clock::time_point> WaitingJob;
typedef std::vector<WaitingJob> JobQueue;

struct MaxResourceNeedAndOldEnough
{
  MaxResourceNeedAndOldEnough(int resMax, std::chrono::system_clock::time_point timePoint) : mResMax(resMax), mTimePoint(timePoint) {}
  bool operator()(const WaitingJob& job)
  {
    return job.first <= mResMax && job.second < mTimePoint;
  }
  const int mResMax;
  const std::chrono::system_clock::time_point mTimePoint;
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
      std::cout << "job arrived with need " << jobRes;
      int nodeId = resMon.findBigEnough(jobRes);
      if (-1 != nodeId)
      {
        std::cout << "\n\n";
        JobMonitor::ResourceDescriptor desc{ { nodeId, jobRes } };
        resMon.claimResource(nodeId, jobRes);
        jobMonitor.addJob(getJobMillisecs(), desc);
      }
      else
      {
        std::cout << ", put in queue\n\n";
        jobQ.push_back({ jobRes, std::chrono::system_clock::now() });
      }
    }

    int nextJobArrivesIn(std::rand() % 10 + 2);
    while (0 < nextJobArrivesIn--)
    {
      //find already spent jobs
      std::vector<int> toRemove;
      for (auto& job : jobMonitor.mJobRecords)
      {
        job.second.first -= 100;
        if (0 >= job.second.first)
        {
          toRemove.push_back(job.first);
        }
      }
      for (int jobId : toRemove)
      {
        jobMonitor.removeJob(jobId);
      }
      //find oldest waiting that fits
      int freeRes(resMon.allFree());
      if (0 < freeRes)
      {
        auto iter(std::find_if(jobQ.begin(), jobQ.end(), MaxResourceNeedAndOldEnough(freeRes, std::chrono::system_clock::now() - std::chrono::milliseconds(400))));
        if (iter != jobQ.end() && iter->first <= freeRes)
        {
          JobMonitor::ResourceDescriptor desc;

          jobRes = iter->first;
          int remainingRes(jobRes);
          while (remainingRes > 0)
          {
            //searching always from the beginning and everything again is not ideal
            int nodeId(resMon.findBigEnough(jobRes));
            if (-1 != nodeId)
            {
              desc.push_back({ nodeId, jobRes });
              resMon.claimResource(nodeId, jobRes);
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
