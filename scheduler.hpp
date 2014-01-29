#pragma once

class ResourceMonitor;

void scheduler(const char* jobsFile, ResourceMonitor* resMonPtr);
void removeJob(int jobId);
