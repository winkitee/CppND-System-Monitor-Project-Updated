#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  vector<string> jiffies = LinuxParser::ProcessUtilization(Pid());
  float starttime = stof(jiffies[LinuxParser::ProcessStates::kStarttime_]);
  float uptime = static_cast<float>(LinuxParser::UpTime());
  hz_ = static_cast<float>(sysconf(_SC_CLK_TCK));
  seconds_ = uptime - (starttime / hz_);
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  float totalTime = static_cast<float>(LinuxParser::ActiveJiffies(Pid()));
  vector<string> jiffies = LinuxParser::ProcessUtilization(Pid());
  float cutime = stof(jiffies[LinuxParser::ProcessStates::kCutime_]);
  float cstime = stof(jiffies[LinuxParser::ProcessStates::kCstime_]);
  totalTime += cutime + cstime;
  float cpuUtilization = 100 * (totalTime / hz_) / seconds_;
  if (cpuUtilization < 0) { return 0.0; }
  cpuUtilization_ = cpuUtilization;
  return cpuUtilization;
}

// DONE: Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(Pid());
}

// DONE: Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(Pid());
}

// DONE: Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(Pid());
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(Pid());
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpuUtilization_ > a.cpuUtilization_;
}
