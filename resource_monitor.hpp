#pragma once

#include <iostream>
#include <mutex>
#include <map>

struct ResourceMonitor
{
  ResourceMonitor(){}
  void addResource(int nodeId)
  {
    mResourceMutex.lock();

    ++mData[nodeId].first;
    ++mData[nodeId].second;

    mResourceMutex.unlock();
  }

  void freeResource(int nodeId, int res)
  {
    mResourceMutex.lock();

    mData[nodeId].second += res;

    mResourceMutex.unlock();
  }

  void claimResource(int nodeId, int res)
  {
    mResourceMutex.lock();

    mData[nodeId].second -= res;

    mResourceMutex.unlock();
  }

  int findFreeInOne(int res)
  {
    return 0;
  }

  void print()
  {
    mResourceMutex.lock();
    for (const auto& resPair : mData)
    {
      std::cout << resPair.first << ": " << resPair.second.second << " / " << resPair.second.first << '\n';
    }
    mResourceMutex.unlock();
  }

  std::mutex mResourceMutex;
  //(nodeID, (all, free))
  typedef std::map<int, std::pair<int, int>> ResourceMonitorData;
  ResourceMonitorData mData;

  ResourceMonitor(const ResourceMonitor&);
};

