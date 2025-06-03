#pragma once

#include "Bar.hpp"

bar_1min::timestamp parse_rfc3339_utc_timestamp(const std::string &timestamp);

bar_1min create_bar_from_csv_line(const std::string &line);

std::vector<bar_1min> create_bars_from_csv(const std::string &csvPath);
