#include "AlpacaWSMarketFeed.hpp"
#include "Bar.hpp"
#include "LoggingUtils.hpp"

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <unistd.h>

class AlpacaWSMarketFeedTest : public ::testing::Test
{
protected:

  void
  SetUp() override
  {
    _ioc = std::make_unique<asio::io_context>();
    CLASS_LOGGER(AlpacaWSMarketFeedTest);
  }

  void
  TearDown() override
  {
    _ioc.reset();
  }

  std::unique_ptr<asio::io_context> _ioc;
};

TEST_F(AlpacaWSMarketFeedTest, ConnectsToFAKEPACAStream)
{
  std::atomic received_bar { false };
  std::atomic bar_count { 0 };
  Bar1min     latest_bar { "", 0, 0, 0, 0, 0, {} };

  const char *api_key    = std::getenv("ALPACA_API_KEY");
  const char *api_secret = std::getenv("ALPACA_API_SECRET");

  if (!api_key || !api_secret)
    {
      GTEST_FAIL() << "Set ALPACA_API_KEY and ALPACA_API_SECRET environment "
                      "variables to run this test";
    }

  AlpacaWSMarketFeed::config config { .api_key    = api_key,
                                      .api_secret = api_secret,
                                      .test_mode  = true };

  AlpacaWSMarketFeed feed { *_ioc, config };

  auto connection = feed.connect_bar_handler([&](const Bar1min &b) {
    latest_bar   = b;
    received_bar = true;
    ++bar_count;
    LOG_INFO(AlpacaWSMarketFeedTest, ConnectsToFAKEPACAStream)
      << "Received bar for " << b.symbol() << " - O: " << b.open()
      << " H: " << b.high() << " L: " << b.low() << " C: " << b.close()
      << " V: " << b.volume();
  });

  feed.start();
  feed.subscribe_to_bars({ "FAKEPACA" });

  std::thread io_thread([this]() {
    _ioc->run();
  });

  const auto     start_time = std::chrono::steady_clock::now();
  constexpr auto timeout    = std::chrono::seconds(60);

  while (!received_bar
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

  EXPECT_TRUE(received_bar.load())
    << "Should have received at least one bar from FAKEPACA stream";
  EXPECT_GT(bar_count.load(), 0) << "Bar count should be greater than 0";
  EXPECT_EQ(latest_bar.symbol(), "FAKEPACA") << "Symbol should be FAKEPACA";
  EXPECT_GT(latest_bar.open(), 0) << "Open price should be greater than 0";
  EXPECT_GT(latest_bar.high(), 0) << "High price should be greater than 0";
  EXPECT_GT(latest_bar.low(), 0) << "Low price should be greater than 0";
  EXPECT_GT(latest_bar.close(), 0) << "Close price should be greater than 0";
  EXPECT_GT(latest_bar.volume(), 0) << "Volume should be greater than 0";
}

TEST_F(AlpacaWSMarketFeedTest, HistoricalDataFeedTest)
{
  std::atomic              received_bars { 0 };
  std::vector<std::string> bar_strings;

  const std::string script_path = "test-utils/run_historical_client.sh";

  if (!std::filesystem::exists(script_path))
    {
      GTEST_FAIL() << "Script not found: " << script_path;
    }

  std::ostringstream start_cmd;
  start_cmd << script_path << " PLTR 2025-05-19 2025-05-23 &";

  if (const int result = std::system(start_cmd.str().c_str()); result != 0)
    {
      GTEST_FAIL() << "Failed to start historical client script";
    }

  std::this_thread::sleep_for(std::chrono::seconds(3));

  const AlpacaWSMarketFeed::config config { .api_key    = "test_key",
                                            .api_secret = "test_secret",
                                            .host       = "localhost",
                                            .port       = "8765",
                                            .test_mode  = false };

  AlpacaWSMarketFeed feed { *_ioc, config };

  auto connection = feed.connect_bar_handler([&](const Bar1min &b) {
    ++received_bars;

    std::stringstream ss;
    ss << "Bar{symbol: " << b.symbol() << ", open: " << b.open()
       << ", high: " << b.high() << ", low: " << b.low()
       << ", close: " << b.close() << ", volume: " << b.volume() << "}";

    bar_strings.push_back(ss.str());

    LOG_INFO(AlpacaWSMarketFeedTest, HistoricalDataFeedTest)
      << "Bar " << received_bars.load() << ": " << ss.str();

    if (received_bars.load() >= 10)
      {
        feed.stop();
      }
  });

  feed.start();
  feed.subscribe_to_bars({ "PLTR" });

  std::thread io_thread([this]() {
    _ioc->run();
  });

  const auto     start_time = std::chrono::steady_clock::now();
  constexpr auto timeout    = std::chrono::seconds(30);

  while (received_bars.load() < 10
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

  std::system("pkill -f run_historical_client.sh || true");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  EXPECT_EQ(received_bars.load(), 10)
    << "Should have received exactly 10 bars";
  EXPECT_EQ(bar_strings.size(), 10) << "Should have 10 bar strings";

  for (size_t i = 0; i < bar_strings.size(); ++i)
    {
      EXPECT_FALSE(bar_strings[i].empty())
        << "Bar string " << i << " should not be empty";
      EXPECT_NE(bar_strings[i].find("PLTR"), std::string::npos)
        << "Bar string " << i << " should contain symbol PLTR";
    }
}
