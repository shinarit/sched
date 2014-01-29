// thread example
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <fstream>

#include <cstdlib>
#include <ctime>

#include "resource_monitor.hpp"

void receiveResources(const char* resFile, ResourceMonitor* resMonPtr)
{
  ResourceMonitor& resMon(*resMonPtr);
  std::ifstream in(resFile);
  int nodeId;
  while (in >> nodeId)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 10 * 100));
//    std::cout << nodeId << '\n';


    resMon.addResource(nodeId);

    resMon.print();
    std::cout << '\n';

  }
}

int main(int argc, char* argv[]) 
{
  std::srand(std::time(0));
  ResourceMonitor resMon;

  std::thread first(receiveResources, argv[1], &resMon);
  first.join();                // pauses until first finishes
}
