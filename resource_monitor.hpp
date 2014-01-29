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

