#pragma once

#include <map>
#include <vector>
#include <mutex>
#include <thread>

#include "resource_monitor.hpp"

struct JobMonitor
{
  //(nodeId, amount)*
  typedef std::vector<std::pair<int, int>> ResourceDescriptor;

  JobMonitor(ResourceMonitor& resMon) : mResMon(resMon) {}

  void addJob(int millisecs, const ResourceDescriptor& resDesc)
  {
    Lock lock(mJobMutex);

    mJobRecords[idCounter] = std::make_pair(millisecs, resDesc);
    ++idCounter;

    std::cout << "started job(" << idCounter - 1 << ") for " << millisecs << " millisec with resources:\n";
    for (const auto& resPair : resDesc)
    {
      std::cout << "  " << resPair.first << ": " << resPair.second << '\n';
    }
    std::cout << '\n';
  }

  void removeJob(int jobId)
  {
    Lock lock(mJobMutex);

    auto& jobDescriptor(mJobRecords[jobId]);
    
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

  //(id, (time_to_run, <resdesc>))
  typedef std::map<int, std::pair<int, ResourceDescriptor>> JobRecords;
  JobRecords mJobRecords;
};
