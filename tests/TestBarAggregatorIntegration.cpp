#include "AlpacaWSMarketFeed.hpp"
#include "Bar.hpp"
#include "BarAggregator.hpp"
#include "LoggingUtils.hpp"

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <filesystem>
#include <gtest/gtest.h>
#include <sstream>
#include <thread>

using namespace std::chrono;

class BarAggregatorIntegrationTest : public ::testing::Test
{
protected:

  void
  SetUp() override
  {
    _ioc = std::make_unique<asio::io_context>();
    CLASS_LOGGER(BarAggregatorIntegrationTest);
  }

  void
  TearDown() override
  {
    cleanup_historical_script();
    _ioc.reset();
  }

  static void
  start_historical_script()
  {
    const std::string script_path = "test-utils/run_historical_client.sh";

    if (!std::filesystem::exists(script_path))
      {
        GTEST_FAIL() << "Script not found: " << script_path;
      }

    std::ostringstream start_cmd;
    start_cmd << script_path << " &";

    if (const int result = std::system(start_cmd.str().c_str()); result != 0)
      {
        GTEST_FAIL() << "Failed to start historical client script";
      }

    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  static void
  cleanup_historical_script()
  {
    std::system("pkill -f run_historical_client.sh || true");
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }

  static AlpacaWSMarketFeed::config
  get_test_config()
  {
    return AlpacaWSMarketFeed::config { .api_key    = "test_key",
                                        .api_secret = "test_secret",
                                        .host       = "localhost",
                                        .port       = "8765",
                                        .test_mode  = false };
  }

  template <typename AggregatorType, typename BarType>
  void
  run_aggregation_test(
    AggregatorType    &aggregator,
    int                minute_bars_to_collect,
    int                expected_aggregated_bars,
    const std::string &test_name)
  {
    std::atomic minute_bar_count { 0 };
    std::atomic aggregated_bar_count { 0 };

    start_historical_script();

    const auto         config = get_test_config();
    AlpacaWSMarketFeed feed { *_ioc, config };

    auto feed_connection = feed.connect_bar_handler([&](const bar_1min &b) {
      ++minute_bar_count;
      LOG_INFO(BarAggregatorIntegrationTest, run_aggregation_test)
        << test_name << " - Received 1-minute bar " << minute_bar_count.load()
        << ": " << b.symbol() << " at " << b.close();
      aggregator.on_bar(b);

      if (minute_bar_count.load() >= minute_bars_to_collect)
        {
          feed.stop();
        }
    });

    auto aggregator_connection
      = aggregator.connect_aggregated_bar_handler([&](const BarType &b) {
          ++aggregated_bar_count;
          LOG_INFO(BarAggregatorIntegrationTest, run_aggregation_test)
            << test_name << " - Received aggregated bar "
            << aggregated_bar_count.load() << ": " << b.symbol() << " at "
            << b.close() << " (vol: " << b.volume() << ")";
        });

    feed.start();
    feed.subscribe_to_bars({ "PLTR" });

    std::thread io_thread([this]() {
      _ioc->run();
    });

    const auto     start_time = std::chrono::steady_clock::now();
    constexpr auto timeout    = std::chrono::seconds(60);

    while (minute_bar_count.load() < minute_bars_to_collect
           && (std::chrono::steady_clock::now() - start_time) < timeout)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

    feed.stop();
    _ioc->stop();

    if (io_thread.joinable())
      {
        io_thread.join();
      }

    EXPECT_EQ(minute_bar_count.load(), minute_bars_to_collect)
      << "Should have received exactly " << minute_bars_to_collect
      << " 1-minute bars";
    EXPECT_EQ(aggregated_bar_count.load(), expected_aggregated_bars)
      << "Should have received exactly " << expected_aggregated_bars
      << " aggregated bars";
  }

  std::unique_ptr<asio::io_context> _ioc;
};

TEST_F(BarAggregatorIntegrationTest, AggregatesHistoricalBarsCorrectly)
{
  BarAggregator<5, minutes> aggregator {};
  constexpr int             minute_bars        = 20;
  constexpr int             expected_5min_bars = minute_bars / 5; // 20 / 5 = 4
  run_aggregation_test<BarAggregator<5, minutes>, bar_5min>(
    aggregator,
    minute_bars,
    expected_5min_bars,
    "AggregatesHistoricalBarsCorrectly");
}

TEST_F(BarAggregatorIntegrationTest, AggregatesHistoricalBarsToHourCorrectly)
{
  BarAggregator<1, hours> aggregator {};
  constexpr int           minute_bars = 65;
  constexpr int           expected_1h_bars
    = minute_bars / 60; // 65 / 60 = 1 (integer division)
  run_aggregation_test<BarAggregator<1, hours>, bar_1h>(
    aggregator,
    minute_bars,
    expected_1h_bars,
    "AggregatesHistoricalBarsToHourCorrectly");
}
