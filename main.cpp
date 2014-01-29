// thread example
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <fstream>

#include <cstdlib>
#include <ctime>

#include "resource_monitor.hpp"
#include "scheduler.hpp"

void receiveResources(const char* resFile, ResourceMonitor* resMonPtr)
{
  ResourceMonitor& resMon(*resMonPtr);
  std::ifstream in(resFile);
  int nodeId;
  while (in >> nodeId)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 10 * 100));

    resMon.addResource(nodeId);

    //resMon.print();
    std::cout << '\n';
  }
}

//usage: <programBinary> <resourceFile> <jobfile>
int main(int argc, char* argv[]) 
{
  std::srand(std::time(0));
  ResourceMonitor resMon;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::thread resourceReceive(receiveResources, argv[1], &resMon);
  std::thread scheduleAndReceiveJobs(scheduler, argv[2], &resMon);
  resourceReceive.join();
  scheduleAndReceiveJobs.join();
}
