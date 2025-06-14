#include "async_rest_client/async_rest_client.hpp"
#include "async_rest_client/typed_task.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/beast/_experimental/test/stream.hpp>
#include <gtest/gtest.h>
#include <string>

class AsyncRestClientConnectTest : public ::testing::Test
{
protected:

  void
  SetUp() override
  {
    _client = async_rest_client::async_rest_client::create(_ioc);
  }

  void
  TearDown() override
  {
    _client.reset();
  }

  async_rest_client::net::io_context                    _ioc {};
  std::shared_ptr<async_rest_client::async_rest_client> _client { nullptr };
};

TEST_F(AsyncRestClientConnectTest, ConnectRequiresScheme)
{
  auto connect_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      const auto ec = co_await _client->connect("httpbin.org/get");
      EXPECT_TRUE(ec);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}

TEST_F(AsyncRestClientConnectTest, ConnectRejectsUnsupportedScheme)
{
  auto connect_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      const auto ec = co_await _client->connect("ftp://example.com");
      EXPECT_TRUE(ec);
      EXPECT_EQ(ec, boost::system::errc::protocol_not_supported);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}

TEST_F(AsyncRestClientConnectTest, ConnectToRealEndpointSucceeds)
{
  auto connect_future { boost::asio::co_spawn(
    _ioc,
    std::bind_front(
      &async_rest_client::async_rest_client::connect,
      _client,
      "https://httpbin.org/get"),
    boost::asio::use_future) };

  _ioc.run();

  EXPECT_FALSE(connect_future.get());
}

TEST_F(AsyncRestClientConnectTest, ConcurrentConnectCalls)
{
  auto connect_future1 { boost::asio::co_spawn(
    _ioc,
    std::bind_front(
      &async_rest_client::async_rest_client::connect,
      _client,
      "https://httpbin.org"),
    boost::asio::use_future) };

  auto connect_future2 { boost::asio::co_spawn(
    _ioc,
    std::bind_front(
      &async_rest_client::async_rest_client::connect,
      _client,
      "https://httpbin.org"),
    boost::asio::use_future) };

  _ioc.run();

  auto ec1 = connect_future1.get();
  auto ec2 = connect_future2.get();

  EXPECT_TRUE((ec1 && !ec2) || (!ec1 && ec2));
}

TEST_F(AsyncRestClientConnectTest, SequentialConnectionsToDifferentHosts)
{
  auto connect_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      const auto ec1 = co_await _client->connect("https://httpbin.org");
      EXPECT_FALSE(ec1);

      const auto ec2 = co_await _client->connect("https://google.com");
      EXPECT_FALSE(ec2);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}
