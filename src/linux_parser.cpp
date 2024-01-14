#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stoi;
using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  float memTotal = 0.0;
  float memFree = 0.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTotal += stof(value);
        }
        if (key == "MemAvailable") {
          memFree += stof(value);
        }
      }
    }
  }
  return (memTotal - memFree) / memTotal;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  string uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  std::vector<string> jiffies = CpuUtilization();

  long totalJiffies = 0.0;
  for (string jiffy : jiffies) {
    totalJiffies += stol(jiffy);
  }

  return totalJiffies;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::vector<string> jiffies = ProcessUtilization(pid);
  long utime = stol(jiffies[ProcessStates::kUtime_]);
  long stime = stol(jiffies[ProcessStates::kStime_]);

  return utime + stime;
}


// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  long user = stol(jiffies[CPUStates::kUser_]);
  long nice = stol(jiffies[CPUStates::kNice_]);
  long system = stol(jiffies[CPUStates::kSystem_]);
  long irq = stol(jiffies[CPUStates::kIRQ_]);
  long softirq = stol(jiffies[CPUStates::kSoftIRQ_]);
  long steal = stol(jiffies[CPUStates::kSteal_]);

  return user + nice + system + irq + softirq + steal;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  long idle = stol(jiffies[CPUStates::kIdle_]);
  long iowait = stol(jiffies[CPUStates::kIOwait_]);

  return idle + iowait;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string cpu, value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
      jiffies.push_back(value);
    }
  }
  return jiffies;
}

vector<string> LinuxParser::ProcessUtilization(int pid) {
  string line;
  string value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  return values;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      if (linestream >> command) {
        return command;
      }
    }
  }
  return string();
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmRSS" || key == "VmSize") {
          return to_string(stol(value) / 1024);
        }
      }
    }
  }
  return "0";
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return "0";
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string name, x, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      linestream >> name >> x >> uid;
      if (uid == Uid(pid)) {
        return name;
      }
    }
  }
  return "DEFAULT"; 
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string starttime;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);

      for (int i = 1; i <= 21; i++) {
        string dummy;
        linestream >> dummy;
      }

      linestream >> starttime;

      return UpTime() - stol(starttime) / sysconf(_SC_CLK_TCK);
    }
  }
  return 0;
}

