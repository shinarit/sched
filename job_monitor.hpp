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
    Lock lock(mJobMutex);

    for (const auto& resPair : resDesc)
    {
      mResMon.claimResource(resPair.first, resPair.second);
    }

    mJobRecords[idCounter] = std::make_pair(std::thread(job, idCounter, millisecs, this), resDesc);
    ++idCounter;

    std::cout << "added job(" << idCounter - 1 << ") for " << millisecs << " millisec with resources:\n";
    for (const auto& resPair : resDesc)
    {
      std::cout << "  " << resPair.first << ": " << resPair.second << '\n';
    }
  }

  void removeJob(int jobId)
  {
    Lock lock(mJobMutex);

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

    mJobRecords.erase(jobId);
  }

  ResourceMonitor& mResMon;

  int idCounter = 0;
  typedef std::unique_lock<std::mutex> Lock;
  std::mutex mJobMutex;

  //(id, (thread, <resdesc>))
  typedef std::map<int, std::pair<std::thread, ResourceDescriptor>> JobRecords;
  JobRecords mJobRecords;
};
