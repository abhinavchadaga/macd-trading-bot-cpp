#include <gtest/gtest.h>
#include "AlpacaWSMarketFeed.hpp"
#include <boost/asio.hpp>
#include <chrono>
#include <thread>
#include <atomic>

class AlpacaWSMarketFeedTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    _ioc = std::make_unique<asio::io_context>();
  }

  void TearDown() override
  {
    _ioc.reset();
  }

  std::unique_ptr<asio::io_context> _ioc;
};

TEST_F(AlpacaWSMarketFeedTest, ConnectsToFAKEPACAStream)
{
  std::atomic received_bar{false};
  std::atomic bar_count{0};
  bar latest_bar{"", 0, 0, 0, 0, 0, {}};

  const char* api_key = std::getenv("ALPACA_API_KEY");
  const char* api_secret = std::getenv("ALPACA_API_SECRET");
  
  if (!api_key || !api_secret) {
    GTEST_SKIP() << "Set ALPACA_API_KEY and ALPACA_API_SECRET environment variables to run this test";
  }

  AlpacaWSMarketFeed::config config{
    .api_key = api_key,
    .api_secret = api_secret, 
    .test_mode = true
  };

  AlpacaWSMarketFeed feed{*_ioc, config};

  auto connection = feed.connect_bar_handler(
    [&](const bar& b) {
      latest_bar = b;
      received_bar = true;
      ++bar_count;
      std::cout << "Received bar for " << b.symbol() 
                << " - O: " << b.open() 
                << " H: " << b.high()
                << " L: " << b.low() 
                << " C: " << b.close()
                << " V: " << b.volume() << std::endl;
    }
  );

  feed.start();
  feed.subscribe_to_bars({"FAKEPACA"});

  std::thread io_thread([this]() {
    _ioc->run();
  });

  const auto start_time = std::chrono::steady_clock::now();
  const auto timeout = std::chrono::seconds(30);

  while (!received_bar && 
         (std::chrono::steady_clock::now() - start_time) < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  feed.stop();
  _ioc->stop();
  
  if (io_thread.joinable()) {
    io_thread.join();
  }

  EXPECT_TRUE(received_bar.load()) << "Should have received at least one bar from FAKEPACA stream";
  EXPECT_GT(bar_count.load(), 0) << "Bar count should be greater than 0";
  EXPECT_EQ(latest_bar.symbol(), "FAKEPACA") << "Symbol should be FAKEPACA";
  EXPECT_GT(latest_bar.open(), 0) << "Open price should be greater than 0";
  EXPECT_GT(latest_bar.high(), 0) << "High price should be greater than 0";
  EXPECT_GT(latest_bar.low(), 0) << "Low price should be greater than 0";
  EXPECT_GT(latest_bar.close(), 0) << "Close price should be greater than 0";
  EXPECT_GT(latest_bar.volume(), 0) << "Volume should be greater than 0";
}

TEST_F(AlpacaWSMarketFeedTest, TestModeUsesCorrectEndpoint)
{
  AlpacaWSMarketFeed::config config{
    .api_key = "test_key",
    .api_secret = "test_secret",
    .test_mode = true
  };

  AlpacaWSMarketFeed feed{*_ioc, config};
  
  std::string expected_url = "wss://stream.data.alpaca.markets/v2/test";
  EXPECT_EQ(feed.get_websocket_url(), expected_url);
}

TEST_F(AlpacaWSMarketFeedTest, SandboxModeUsesCorrectEndpoint)
{
  AlpacaWSMarketFeed::config config{
    .api_key = "test_key",
    .api_secret = "test_secret",
    .sandbox = true,
    .test_mode = false
  };

  AlpacaWSMarketFeed feed{*_ioc, config};
  
  std::string expected_url = "wss://stream.data.sandbox.alpaca.markets/v2/iex";
  EXPECT_EQ(feed.get_websocket_url(), expected_url);
}

TEST_F(AlpacaWSMarketFeedTest, ProductionModeUsesCorrectEndpoint)
{
  AlpacaWSMarketFeed::config config{
    .api_key = "test_key",
    .api_secret = "test_secret",
    .feed = "sip",
    .sandbox = false,
    .test_mode = false
  };

  AlpacaWSMarketFeed feed{*_ioc, config};
  
  std::string expected_url = "wss://stream.data.alpaca.markets/v2/sip";
  EXPECT_EQ(feed.get_websocket_url(), expected_url);
}