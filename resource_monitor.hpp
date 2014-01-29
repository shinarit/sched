#pragma once

#include <iostream>
#include <mutex>
#include <map>
#include <algorithm>

struct ResourceMonitor
{
  ResourceMonitor(){}
  void addResource(int nodeId)
  {
    Lock lock(mResourceMutex);

    ++mData[nodeId].first;
    ++mData[nodeId].second;
    ++mAllFree;
  }

  void freeResource(int nodeId, int res)
  {
    Lock lock(mResourceMutex);

    mData[nodeId].second += res;
    mAllFree += res;
  }

  void claimResource(int nodeId, int res)
  {
    Lock lock(mResourceMutex);

    mData[nodeId].second -= res;
    mAllFree -= res;
  }

  int findBigEnough(int res)
  {
    Lock lock(mResourceMutex);

    if (mAllFree < res)
    {
      return -1;
    }

    auto iter(std::find_if(mData.begin(), mData.end(), [=](const ResourceMonitorData::value_type& pair){ return res <= pair.second.second; }));
    if (iter == mData.end())
    {
      return -1;
    }
    return iter->first;
  }

  int allFree() const
  {
    return mAllFree;
  }

  void print()
  {
    Lock lock(mResourceMutex);
    for (const auto& resPair : mData)
    {
      std::cout << resPair.first << ": " << resPair.second.second << " / " << resPair.second.first << '\n';
    }
  }

  typedef std::unique_lock<std::mutex> Lock;
  int mAllFree = 0;
  std::mutex mResourceMutex;
  //(nodeID, (all, free))
  typedef std::map<int, std::pair<int, int>> ResourceMonitorData;
  //should use a more effective structure
  ResourceMonitorData mData;

  ResourceMonitor(const ResourceMonitor&);
};

