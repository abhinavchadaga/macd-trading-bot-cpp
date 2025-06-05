#pragma once

#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>

namespace HistoricalDataTestUtils
{

inline void
cleanup_historical_server()
{
  std::system("pkill -f historical_alpaca_ws_endpoint || true");
  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::system("pkill -9 -f historical_alpaca_ws_endpoint || true");
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

inline void
start_historical_server(
  const std::string &symbol     = "PLTR",
  const std::string &start_date = "2025-05-19",
  const std::string &end_date   = "2025-05-23",
  const std::string &delay      = "0.1")
{
  cleanup_historical_server();

  const std::string csv_file = "/tmp/" + symbol + "_" + start_date + "_"
                             + end_date + "_1min_market_hours.csv";
  const std::string csv_cmd = "historical_bars_to_csv " + symbol + " "
                            + start_date + " " + end_date + " --output "
                            + csv_file;
  std::system(csv_cmd.c_str());

  const std::string server_cmd
    = "historical_alpaca_ws_endpoint " + csv_file + " --delay " + delay + " &";
  std::system(server_cmd.c_str());

  std::this_thread::sleep_for(std::chrono::seconds(3));
}

} // namespace HistoricalDataTestUtils
