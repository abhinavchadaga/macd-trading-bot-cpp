#include "Utils.hpp"

#include "Bar.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

Bar1min::Timestamp
parse_helper(const std::string &timestamp, const char *fmt_string)
{
  std::stringstream ss { timestamp };

  std::tm tm {};
  ss >> std::get_time(&tm, fmt_string);

  time_t epoch_ns { timegm(&tm) };
  auto   ts { std::chrono::system_clock::from_time_t(epoch_ns) };

  return { std::chrono::time_point_cast<std::chrono::minutes>(ts) };
}

Bar1min::Timestamp
parseRFC3339UTCTimestamp(const std::string &timestamp)
{
  if (timestamp.ends_with('Z') || timestamp.ends_with('z'))
    {
      // Format: 2025-05-19T13:30:00Z
      return parse_helper(timestamp, "%Y-%m-%dT%H:%M:%S");
    }
  else if (timestamp.ends_with("+00:00"))
    {
      // Format: 2025-05-19 13:30:00+00:00
      return parse_helper(timestamp, "%Y-%m-%d %H:%M:%S");
    }
  else
    {
      throw std::invalid_argument {
        "timestamp must be UTC (end with 'Z' or '+00:00')"
      };
    }

  return {};
}

Bar1min
createBarFromCSVLine(const std::string &line)
{
  std::istringstream       ss { line };
  std::string              token;
  std::vector<std::string> tokens;

  while (std::getline(ss, token, ','))
    {
      tokens.push_back(token);
    }

  if (tokens.size() < 7)
    {
      throw std::invalid_argument { "CSV line must have at least 7 columns" };
    }

  std::string symbol    = tokens[0];
  std::string timestamp = tokens[1];
  double      open      = std::stod(tokens[2]);
  double      high      = std::stod(tokens[3]);
  double      low       = std::stod(tokens[4]);
  double      close     = std::stod(tokens[5]);
  auto        volume    = static_cast<uint64_t>(std::stod(tokens[6]));

  Bar1min::Timestamp ts = parseRFC3339UTCTimestamp(timestamp);

  return Bar1min { std::move(symbol), open, high, low, close, volume, ts };
}

std::vector<Bar1min>
createBarsFromCSV(const std::string &csvPath)
{
  std::ifstream file { csvPath };
  if (!file.is_open())
    {
      throw std::runtime_error { "Failed to open CSV file: " + csvPath };
    }

  std::vector<Bar1min> bars;
  std::string          line;

  if (std::getline(file, line))
    {
      // Skip header line
    }

  while (std::getline(file, line))
    {
      if (!line.empty())
        {
          bars.push_back(createBarFromCSVLine(line));
        }
    }

  return bars;
}
