#include "LoggingUtils.hpp"
#include "alpaca_trade_client/alpaca_trade_client.hpp"
#include "alpaca_trade_client/orders.hpp"

#include <atomic>
#include <boost/asio/ssl.hpp>
#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <thread>

namespace net = boost::asio;
namespace ssl = net::ssl;

class AlpacaTradeClientTest : public ::testing::Test
{
protected:

  void
  SetUp() override
  {
    _ioc     = std::make_unique<net::io_context>();
    _ssl_ctx = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
    _ssl_ctx->set_verify_mode(ssl::verify_peer);
    _ssl_ctx->set_default_verify_paths();

    CLASS_LOGGER(AlpacaTradeClientTest);
  }

  void
  TearDown() override
  {
    _ioc.reset();
    _ssl_ctx.reset();
  }

  std::unique_ptr<net::io_context> _ioc;
  std::unique_ptr<ssl::context>    _ssl_ctx;
};

TEST_F(AlpacaTradeClientTest, ConnectsToAlpacaPaperEndpointSuccessfully)
{
  const char *api_key { std::getenv("ALPACA_API_KEY") };
  const char *api_secret { std::getenv("ALPACA_API_SECRET") };

  if (!api_key || !api_secret)
    {
      GTEST_SKIP() << "Set ALPACA_API_KEY and ALPACA_API_SECRET environment "
                      "variables to run this test";
    }

  auto client { alpaca_trade_client::create(*_ioc, *_ssl_ctx) };

  alpaca_trade_client::config cfg {};
  cfg.api_key    = api_key;
  cfg.secret_key = api_secret;
  cfg.endpoint   = "https://paper-api.alpaca.markets";
  cfg.timeout    = std::chrono::seconds { 10 };

  EXPECT_FALSE(client->is_connected())
    << "Client should not be connected initially";

  EXPECT_NO_THROW({ client->connect(cfg); })
    << "Connection to valid Alpaca paper endpoint should succeed";

  EXPECT_TRUE(client->is_connected())
    << "Client should be connected after successful connect()";

  EXPECT_NO_THROW({ client->disconnect(); }) << "Disconnect should succeed";

  EXPECT_FALSE(client->is_connected())
    << "Client should not be connected after disconnect()";
}

TEST_F(AlpacaTradeClientTest, ThrowsErrorForInvalidEndpoint)
{
  auto client { alpaca_trade_client::create(*_ioc, *_ssl_ctx) };

  alpaca_trade_client::config cfg {};
  cfg.api_key    = "test_key";
  cfg.secret_key = "test_secret";
  cfg.endpoint   = "https://invalid-nonexistent-endpoint.com";
  cfg.timeout    = std::chrono::seconds { 5 };

  EXPECT_FALSE(client->is_connected())
    << "Client should not be connected initially";

  EXPECT_THROW(
    { client->connect(cfg); },
    std::runtime_error)
    << "Connection to invalid endpoint should throw exception";

  EXPECT_FALSE(client->is_connected())
    << "Client should remain disconnected after failed connection attempt";
}

TEST_F(AlpacaTradeClientTest, DisconnectWhenNotConnectedLogsWarning)
{
  auto client { alpaca_trade_client::create(*_ioc, *_ssl_ctx) };

  EXPECT_FALSE(client->is_connected())
    << "Client should not be connected initially";

  EXPECT_NO_THROW({ client->disconnect(); })
    << "Disconnect when not connected should not throw";

  EXPECT_FALSE(client->is_connected()) << "Client should remain disconnected";
}

TEST_F(AlpacaTradeClientTest, SubmitMarketOrderForPLTRWithExtendedHours)
{
  const char *api_key { std::getenv("ALPACA_API_KEY") };
  const char *api_secret { std::getenv("ALPACA_API_SECRET") };

  if (!api_key || !api_secret)
    {
      GTEST_SKIP() << "Set ALPACA_API_KEY and ALPACA_API_SECRET environment "
                      "variables to run this test";
    }

  auto client { alpaca_trade_client::create(*_ioc, *_ssl_ctx) };

  alpaca_trade_client::config cfg {};
  cfg.api_key    = api_key;
  cfg.secret_key = api_secret;
  cfg.endpoint   = "https://paper-api.alpaca.markets";
  cfg.timeout    = std::chrono::seconds { 10 };

  client->connect(cfg);
  ASSERT_TRUE(client->is_connected())
    << "Client must be connected to submit order";

  limit_order order {};
  order.symbol         = "PLTR";
  order.qty            = "1";
  order.side           = OrderSide::BUY;
  order.time_in_force  = TimeInForce::DAY;
  order.limit_price    = "10000.00"; // this is never going to be filled
  order.extended_hours = true;

  std::atomic<bool>         order_completed { false };
  boost::system::error_code received_error {};
  nlohmann::json            received_response {};

  client->submit_order(
    order,
    [&](boost::system::error_code ec, const nlohmann::json &response) {
      received_error    = ec;
      received_response = response;
      order_completed   = true;
    });

  std::thread io_thread([this]() {
    _ioc->run();
  });

  const auto     start_time = std::chrono::steady_clock::now();
  constexpr auto timeout    = std::chrono::seconds(30);

  while (!order_completed
         && (std::chrono::steady_clock::now() - start_time) < timeout)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  _ioc->stop();

  if (io_thread.joinable())
    {
      io_thread.join();
    }

  EXPECT_TRUE(order_completed.load())
    << "Order completion handler should be called";

  if (received_error)
    {
      FAIL() << "Order submission failed with error: "
             << received_error.message()
             << " (code: " << received_error.value() << ")";
    }

  EXPECT_FALSE(received_response.empty()) << "Response should not be empty";

  LOG_DEBUG(AlpacaTradeClientTest, SubmitMarketOrderForPLTRWithExtendedHours)
    << "Received response: " << received_response.dump();

  if (!received_response.empty())
    {
      EXPECT_TRUE(received_response.is_array())
        << "Response should be a JSON array";
      EXPECT_EQ(received_response.size(), 1)
        << "Response array should contain exactly one order";

      if (received_response.is_array() && received_response.size() == 1)
        {
          const auto &order_obj = received_response.at(0);

          EXPECT_TRUE(order_obj.is_object())
            << "Extracted order should be a JSON object";

          EXPECT_TRUE(order_obj.contains("id"))
            << "Order should contain order ID";
          EXPECT_TRUE(order_obj.contains("symbol"))
            << "Order should contain symbol";
          EXPECT_EQ(order_obj["symbol"], "PLTR")
            << "Order symbol should match order symbol";
          EXPECT_TRUE(order_obj.contains("side"))
            << "Order should contain side";
          EXPECT_EQ(order_obj["side"], "buy")
            << "Order side should match order side";
          EXPECT_TRUE(order_obj.contains("qty"))
            << "Order should contain quantity";
          EXPECT_EQ(order_obj["qty"], "1")
            << "Order quantity should match order quantity";
          EXPECT_TRUE(order_obj.contains("extended_hours"))
            << "Order should contain extended_hours flag";
          EXPECT_TRUE(order_obj["extended_hours"])
            << "Order extended_hours should be true";
        }
    }

  client->disconnect();
}
