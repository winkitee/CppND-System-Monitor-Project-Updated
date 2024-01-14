#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long activeJiffies = LinuxParser::ActiveJiffies();
  long idleJiffies = LinuxParser::IdleJiffies();

  long prevTotal = prevActiveJiffies + prevIdleJiffies;
  long total = activeJiffies + idleJiffies;

  float totald = static_cast<float>(total - prevTotal);
  float idled = static_cast<float>(idleJiffies - prevIdleJiffies);

  prevActiveJiffies = activeJiffies;
  prevIdleJiffies = idleJiffies;

  return (totald - idled) / totald;
}