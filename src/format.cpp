#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int oneHour = 60 * 60;
  int oneMinute = 60;

  long h = seconds / oneHour;
  seconds = seconds % oneHour;
  long m = seconds / oneMinute;
  seconds = seconds % oneMinute;

  string hour = std::to_string(h);
  if (hour.size() < 2) { hour = "0" + hour; }
  string minute = std::to_string(m);
  if (minute.size() < 2) { minute = "0" + minute; }
  string second = std::to_string(seconds);
  if (second.size() < 2) { second = "0" + second; }

  return hour + ":" + minute + ":" + second;
}

