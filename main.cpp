// thread example
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <fstream>

#include <map>

#include <cstdlib>
#include <ctime>


//(nodeID, (all, free))
typedef std::map<int, std::pair<int, int>> ResourceMonitor;

std::mutex ResourceMutex;

void printResourceMonitor(const ResourceMonitor& resMon);

void receiveResources(const char* resFile, ResourceMonitor& resMon)
{
  std::ifstream in(resFile);
  int nodeId;
  while (in >> nodeId)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 10 * 100));
//    std::cout << nodeId << '\n';

    ResourceMutex.lock();

    ++resMon[nodeId].first;
    ++resMon[nodeId].second;

    printResourceMonitor(resMon);
    std::cout << '\n';

    ResourceMutex.unlock();
  }
}

void printResourceMonitor(const ResourceMonitor& resMon)
{
  for (const auto& resPair : resMon)
  {
    std::cout << resPair.first << ": " << resPair.second.second << " / " << resPair.second.first << '\n';
  }
}

int main(int argc, char* argv[]) 
{
  std::srand(std::time(0));
  ResourceMonitor resMon;

  std::thread first(receiveResources, argv[1], resMon);
  first.join();                // pauses until first finishes
}
