#include "LoggingUtils.hpp"
#include "alpaca_trade_client.hpp"

#include <boost/asio/ssl.hpp>
#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

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
