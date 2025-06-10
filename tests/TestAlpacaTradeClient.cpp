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

  static void
  skip_if_environment_variables_missing()
  {
    const char *api_key { std::getenv("ALPACA_API_KEY") };
    const char *api_secret { std::getenv("ALPACA_API_SECRET") };

    if (!api_key || !api_secret)
      {
        GTEST_SKIP() << "Set ALPACA_API_KEY and ALPACA_API_SECRET environment "
                        "variables to run this test";
      }
  }

  static alpaca_trade_client::config
  create_test_config()
  {
    const char *api_key { std::getenv("ALPACA_API_KEY") };
    const char *api_secret { std::getenv("ALPACA_API_SECRET") };

    alpaca_trade_client::config cfg {};
    cfg.api_key    = api_key;
    cfg.secret_key = api_secret;
    cfg.endpoint   = "https://paper-api.alpaca.markets";
    cfg.timeout    = std::chrono::seconds { 10 };

    return cfg;
  }

  std::shared_ptr<alpaca_trade_client>
  create_client()
  {
    return alpaca_trade_client::create(*_ioc, *_ssl_ctx);
  }

  std::shared_ptr<alpaca_trade_client>
  create_connected_client()
  {
    skip_if_environment_variables_missing();

    auto client { create_client() };
    auto cfg { create_test_config() };

    client->connect(cfg);
    return client;
  }

  std::unique_ptr<net::io_context> _ioc;
  std::unique_ptr<ssl::context>    _ssl_ctx;
};

TEST_F(AlpacaTradeClientTest, ConnectsToAlpacaPaperEndpointSuccessfully)
{
  skip_if_environment_variables_missing();

  auto client { create_client() };
  auto cfg { create_test_config() };

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
  auto client { create_client() };

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
  auto client { create_client() };

  EXPECT_FALSE(client->is_connected())
    << "Client should not be connected initially";

  EXPECT_NO_THROW({ client->disconnect(); })
    << "Disconnect when not connected should not throw";

  EXPECT_FALSE(client->is_connected()) << "Client should remain disconnected";
}

TEST_F(AlpacaTradeClientTest, SubmitMarketOrderForPLTRWithExtendedHours)
{
  auto client { create_connected_client() };

  ASSERT_TRUE(client->is_connected())
    << "Client must be connected to submit order";

  limit_order order {};
  order.symbol         = "PLTR";
  order.qty            = "1";
  order.side           = OrderSide::BUY;
  order.time_in_force  = TimeInForce::GTC;
  order.limit_price    = "0.01";
  order.extended_hours = true;

  boost::system::error_code received_error {};
  nlohmann::json            received_response {};

  std::atomic<bool> order_completed { false };

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
      EXPECT_TRUE(received_response.contains("id"))
        << "Order should contain order ID";
      EXPECT_TRUE(received_response.contains("symbol"))
        << "Order should contain symbol";
      EXPECT_EQ(received_response["symbol"], "PLTR")
        << "Order symbol should match order symbol";
      EXPECT_TRUE(received_response.contains("side"))
        << "Order should contain side";
      EXPECT_EQ(received_response["side"], "buy")
        << "Order side should match order side";
      EXPECT_TRUE(received_response.contains("qty"))
        << "Order should contain quantity";
      EXPECT_EQ(received_response["qty"], "1")
        << "Order quantity should match order quantity";
      EXPECT_TRUE(received_response.contains("extended_hours"))
        << "Order should contain extended_hours flag";
      EXPECT_TRUE(received_response["extended_hours"])
        << "Order extended_hours should be true";
    }

  client->disconnect();
}

TEST_F(AlpacaTradeClientTest, CloseAllPositionsAfterSubmittingOrder)
{
  auto client { create_connected_client() };

  ASSERT_TRUE(client->is_connected())
    << "Client must be connected to close positions";

  boost::system::error_code received_error {};
  nlohmann::json            received_response {};

  std::atomic<bool> close_completed { false };

  client->close_all_positions(
    true,
    [&](boost::system::error_code ec, const nlohmann::json &response) {
      received_error    = ec;
      received_response = response;
      close_completed   = true;
    });

  std::thread io_thread([this]() {
    _ioc->run();
  });

  const auto     start_time = std::chrono::steady_clock::now();
  constexpr auto timeout    = std::chrono::seconds(30);

  while (!close_completed
         && (std::chrono::steady_clock::now() - start_time) < timeout)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  _ioc->stop();

  if (io_thread.joinable())
    {
      io_thread.join();
    }

  EXPECT_TRUE(close_completed.load())
    << "Close positions completion handler should be called";

  if (received_error)
    {
      FAIL() << "Close all positions failed with error: "
             << received_error.message()
             << " (code: " << received_error.value() << ")";
    }

  LOG_DEBUG(AlpacaTradeClientTest, CloseAllPositionsAfterSubmittingOrder)
    << "Received response: " << received_response.dump();

  if (!received_response.empty())
    {
      EXPECT_TRUE(received_response.is_array())
        << "Close all positions response should be an array";

      if (received_response.is_array() && !received_response.empty())
        {
          for (const auto &close_result : received_response)
            {
              EXPECT_TRUE(close_result.contains("symbol"))
                << "Each close result should contain symbol";
              EXPECT_TRUE(close_result.contains("status"))
                << "Each close result should contain status";
            }
        }
    }

  client->disconnect();
}

TEST_F(AlpacaTradeClientTest, VerifyNoOpenPositionsAfterClosingAll)
{
  auto client { create_connected_client() };

  ASSERT_TRUE(client->is_connected())
    << "Client must be connected to get positions";

  boost::system::error_code received_error {};
  nlohmann::json            received_response {};

  std::atomic<bool> positions_completed { false };

  client->get_all_positions(
    [&](boost::system::error_code ec, const nlohmann::json &response) {
      received_error      = ec;
      received_response   = response;
      positions_completed = true;
    });

  std::thread io_thread([this]() {
    _ioc->run();
  });

  const auto     start_time = std::chrono::steady_clock::now();
  constexpr auto timeout    = std::chrono::seconds(30);

  while (!positions_completed
         && (std::chrono::steady_clock::now() - start_time) < timeout)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  _ioc->stop();

  if (io_thread.joinable())
    {
      io_thread.join();
    }

  EXPECT_TRUE(positions_completed.load())
    << "Get all positions completion handler should be called";

  if (received_error)
    {
      FAIL() << "Get all positions failed with error: "
             << received_error.message()
             << " (code: " << received_error.value() << ")";
    }

  LOG_DEBUG(AlpacaTradeClientTest, VerifyNoOpenPositionsAfterClosingAll)
    << "Received response: " << received_response.dump();

  EXPECT_TRUE(received_response.is_array())
    << "Get all positions response should be an array";

  EXPECT_TRUE(received_response.empty())
    << "Should have no open positions after closing all positions";

  client->disconnect();
}
