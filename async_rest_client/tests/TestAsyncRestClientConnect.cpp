#include "async_rest_client/async_rest_client.hpp"
#include "my_logger.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/beast/_experimental/test/stream.hpp>
#include <gtest/gtest.h>
#include <string>

namespace net = boost::asio;
using arc     = async_rest_client::async_rest_client;

class AsyncRestClientConnectTest : public ::testing::Test
{
protected:

  static void
  SetUpTestCase()
  {
    init_logger("async_rest_client");
  }

  void
  SetUp() override
  {
    _client = arc::create(_ioc);
  }

  void
  TearDown() override
  {
    _client.reset();
  }

  net::io_context      _ioc {};
  std::shared_ptr<arc> _client { nullptr };
};

TEST_F(AsyncRestClientConnectTest, ConnectRequiresScheme)
{
  LOG_INFO("Starting AsyncRestClientConnectTest::ConnectRequiresScheme Test");
  auto connect_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> net::awaitable<void> {
      const auto ec = co_await _client->connect("httpbin.org/get");
      EXPECT_TRUE(ec);
    },
    net::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}

TEST_F(AsyncRestClientConnectTest, ConnectRejectsUnsupportedScheme)
{
  LOG_INFO(
    "Starting AsyncRestClientConnectTest::ConnectRejectsUnsupportedScheme "
    "Test");
  auto connect_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> net::awaitable<void> {
      const auto ec = co_await _client->connect("ftp://example.com");
      EXPECT_TRUE(ec);
      EXPECT_EQ(ec, boost::system::errc::protocol_not_supported);
    },
    net::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}

TEST_F(AsyncRestClientConnectTest, ConnectToRealEndpointSucceeds)
{
  LOG_INFO(
    "Starting AsyncRestClientConnectTest::ConnectToRealEndpointSucceeeds "
    "Test");
  auto connect_future { net::co_spawn(
    _ioc,
    std::bind_front(&arc::connect, _client, "https://httpbin.org/get"),
    net::use_future) };

  _ioc.run();

  EXPECT_FALSE(connect_future.get());
}

TEST_F(AsyncRestClientConnectTest, ConcurrentConnectCalls)
{
  LOG_INFO("Starting AsyncRestClientConnectTest::ConcurrentConnectCalls Test");
  auto connect_future1 { net::co_spawn(
    _ioc,
    std::bind_front(&arc::connect, _client, "https://httpbin.org"),
    net::use_future) };

  auto connect_future2 { net::co_spawn(
    _ioc,
    std::bind_front(&arc::connect, _client, "https://google.com"),
    net::use_future) };

  _ioc.run();

  auto ec1 = connect_future1.get();
  auto ec2 = connect_future2.get();

  LOG_INFO("ec1: {}", ec1 ? ec1.message() : "success");
  LOG_INFO("ec2: {}", ec2 ? ec2.message() : "success");

  EXPECT_TRUE((ec1 && !ec2) || (!ec1 && ec2));
}

TEST_F(AsyncRestClientConnectTest, SequentialConnectionsToDifferentHosts)
{
  LOG_INFO(
    "Starting "
    "AsyncRestClientConnectTest::SequentialConnectionsToDifferentHosts Test");
  auto connect_future { net::co_spawn(
    _ioc,
    [this]() -> net::awaitable<void> {
      const auto ec1 = co_await _client->connect("https://httpbin.org");
      EXPECT_FALSE(ec1);

      const auto ec2 = co_await _client->connect("https://google.com");
      EXPECT_FALSE(ec2);
    },
    net::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}
