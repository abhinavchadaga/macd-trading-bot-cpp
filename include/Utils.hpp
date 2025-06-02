#pragma once

#include "Bar.hpp"

Bar1min::Timestamp parseRFC3339UTCTimestamp(const std::string &timestamp);

Bar1min createBarFromCSVLine(const std::string &line);

std::vector<Bar1min> createBarsFromCSV(const std::string &csvPath);
