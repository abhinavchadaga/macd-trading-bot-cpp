#include "Utils.hpp"
#include "Bar.hpp"

#include <chrono>

Bar1min::Timestamp parseRFC3339UTCTimestamp(const std::string& timestamp) {
  if (!timestamp.ends_with('Z') && !timestamp.ends_with('z')) {
    throw std::invalid_argument{"timestamp must be UTC (end with 'Z')"};
  }

  std::chrono::sys_seconds tp{};
  std::istringstream ss{timestamp};
  std::chrono::from_stream(ss, "%Y-%m-%dT%H:%M:%SZ", tp);

  if (ss.fail()) {
    throw std::invalid_argument{"Malformed timestamp"};
  }

  return std::chrono::time_point_cast<std::chrono::minutes>(tp);
}
