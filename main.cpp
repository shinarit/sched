// thread example
#include <iostream>
#include <thread>
#include <chrono>

#include <fstream>

#include <map>

#include <cstdlib>
#include <ctime>

void receiveResources(const char* resFile) 
{
  std::ifstream in(resFile);
  int nodeId;
  while (in >> nodeId)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 10 * 100));
    std::cout << nodeId << '\n';
  }
}

//(nodeID, (all, free))
typedef std::map<int, std::pair<int, int>> ResourceMonitor;

int main(int argc, char* argv[]) 
{
  std::srand(std::time(0));
  ResourceMonitor resMon;

  std::thread first(receiveResources, argv[1]);
  first.join();                // pauses until first finishes
}
