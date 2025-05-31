#include <gtest/gtest.h>
#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <thread>
#include "AlpacaWSMarketFeed.hpp"
#include "BarAggregator.hpp"
#include "LoggingUtils.hpp"

class BarAggregatorIntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    _ioc = std::make_unique<asio::io_context>();
    CLASS_LOGGER(BarAggregatorIntegrationTest);
  }

  void TearDown() override { _ioc.reset(); }

  std::unique_ptr<asio::io_context> _ioc;
};

TEST_F(BarAggregatorIntegrationTest, AggregatesHistoricalBarsCorrectly) {
  std::atomic minute_bar_count{0};
  std::atomic aggregated_bar_count{0};

  const std::string script_path =
      "../../../test-utils/run_historical_client.sh";

  if (!std::filesystem::exists(script_path)) {
    GTEST_FAIL() << "Script not found: " << script_path;
  }

  std::ostringstream start_cmd;
  start_cmd << script_path << " &";

  if (const int result = std::system(start_cmd.str().c_str()); result != 0) {
    GTEST_FAIL() << "Failed to start historical client script";
  }

  std::this_thread::sleep_for(std::chrono::seconds(3));

  const AlpacaWSMarketFeed::config config{.api_key = "test_key",
                                          .api_secret = "test_secret",
                                          .host = "localhost",
                                          .port = "8765",
                                          .test_mode = false};

  AlpacaWSMarketFeed feed{*_ioc, config};
  BarAggregator aggregator{5};  // 5-minute aggregation

  // Connect market feed to aggregator
  auto feed_connection = feed.connect_bar_handler([&](const Bar& b) {
    ++minute_bar_count;
    LOG_INFO(BarAggregatorIntegrationTest, AggregatesHistoricalBarsCorrectly)
        << "Received 1-minute bar " << minute_bar_count.load() << ": "
        << b.symbol() << " at " << b.close();
    aggregator.on_bar(b);

    // Stop after receiving 20 bars
    if (minute_bar_count.load() >= 20) {
      feed.stop();
    }
  });

  // Connect aggregator output handler
  auto aggregator_connection =
      aggregator.connect_aggregated_bar_handler([&](const Bar& b) {
        ++aggregated_bar_count;
        LOG_INFO(BarAggregatorIntegrationTest,
                 AggregatesHistoricalBarsCorrectly)
            << "Received 5-minute aggregated bar "
            << aggregated_bar_count.load() << ": " << b.symbol() << " at "
            << b.close() << " (vol: " << b.volume() << ")";
      });

  feed.start();
  feed.subscribe_to_bars({"PLTR"});

  std::thread io_thread([this]() { _ioc->run(); });

  const auto start_time = std::chrono::steady_clock::now();
  constexpr auto timeout = std::chrono::seconds(60);

  while (minute_bar_count.load() < 20 &&
         (std::chrono::steady_clock::now() - start_time) < timeout) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  feed.stop();
  _ioc->stop();

  if (io_thread.joinable()) {
    io_thread.join();
  }

  std::system("pkill -f run_historical_client.sh || true");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  EXPECT_EQ(minute_bar_count.load(), 20)
      << "Should have received exactly 20 1-minute bars";
  EXPECT_EQ(aggregated_bar_count.load(), 4)
      << "Should have received exactly 4 5-minute aggregated bars";
}
