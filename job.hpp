#pragma once

#include <thread>
#include <chrono>

#include "scheduler.hpp"
class JobMonitor;

void job(int id, int runtimeInMillisec, JobMonitor* jobMon);
