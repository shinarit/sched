#pragma once

#include <map>
#include <vector>
#include <mutex>
#include <thread>

#include "job.hpp"
#include "resource_monitor.hpp"

struct JobMonitor
{
  //(nodeId, amount)*
  typedef std::vector<std::pair<int, int>> ResourceDescriptor;

  JobMonitor(ResourceMonitor& resMon) : mResMon(resMon) {}

  void addJob(int millisecs, const ResourceDescriptor& resDesc)
  {
    mJobMutex.lock();

    for (const auto& resPair : resDesc)
    {
      mResMon.claimResource(resPair.first, resPair.second);
    }

    mJobRecords[idCounter] = std::make_pair(std::thread(job, idCounter, millisecs, this), resDesc);
    ++idCounter;

    std::cout << "added job with resources: ";
    for (const auto& resPair : resDesc)
    {
      std::cout << "  " << resPair.first << ": " << resPair.second << '\n';
    }

    mJobMutex.unlock();
  }

  void removeJob(int jobId)
  {
    mJobMutex.lock();

    auto& jobDescriptor(mJobRecords[jobId]);

    if (jobDescriptor.first.joinable())
    {
      jobDescriptor.first.join();
    }

    for (const auto& resPair : jobDescriptor.second)
    {
      mResMon.freeResource(resPair.first, resPair.second);
    }

    std::cout << "finished job. resource state:\n";
    mResMon.print();

    mJobMutex.unlock();
  }

  ResourceMonitor& mResMon;

  int idCounter = 0;
  std::mutex mJobMutex;

  //(id, (thread, <resdesc>))
  typedef std::map<int, std::pair<std::thread, ResourceDescriptor>> JobRecords;
  JobRecords mJobRecords;
};
