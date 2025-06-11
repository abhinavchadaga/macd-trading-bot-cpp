#include "async_rest_client/async_rest_client.hpp"
#include "async_rest_client/typed_task.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>

TEST(AsyncRestClientTest, CanCreateClient)
{
  boost::asio::io_context ioc {};
  const auto client = async_rest_client::async_rest_client::create(ioc);
  ASSERT_NE(client, nullptr);
}

TEST(AsyncRestClientTest, CanInstantiateConnectionContext)
{
  boost::asio::io_context               ioc {};
  async_rest_client::beast::tcp_stream  tcp_stream { ioc };
  async_rest_client::beast::flat_buffer buffer {};

  const async_rest_client::http_connection_context http_ctx { tcp_stream,
                                                              buffer };
  EXPECT_EQ(&http_ctx.stream, &tcp_stream);
  EXPECT_EQ(&http_ctx.buffer, &buffer);
}

TEST(AsyncRestClientTest, ConceptsWork)
{
  static_assert(async_rest_client::SupportedStreamType<
                async_rest_client::beast::tcp_stream>);
  static_assert(async_rest_client::SupportedStreamType<
                async_rest_client::beast::ssl_stream<
                  async_rest_client::beast::tcp_stream>>);

  static_assert(async_rest_client::SupportedRequestBody<
                async_rest_client::http::empty_body>);
  static_assert(async_rest_client::SupportedRequestBody<
                async_rest_client::http::string_body>);

  static_assert(async_rest_client::SupportedResponseBody<
                async_rest_client::http::string_body>);

  static_assert(!async_rest_client::SupportedRequestBody<
                async_rest_client::http::file_body>);
  static_assert(!async_rest_client::SupportedResponseBody<
                async_rest_client::http::empty_body>);
}

TEST(AsyncRestClientTest, CanInstantiateTypedTask)
{
  async_rest_client::http::fields headers;

  using TaskType = async_rest_client::typed_task<
    async_rest_client::http::empty_body,
    async_rest_client::http::string_body>;
  TaskType task { "https://httpbin.org/get",
                  async_rest_client::http::verb::get,
                  headers,
                  async_rest_client::http::empty_body::value_type {} };

  auto future = task.get_shared_future();
  EXPECT_TRUE(future.valid());
}

TEST(AsyncRestClientTest, ConnectRequiresScheme)
{
  boost::asio::io_context ioc {};
  auto client { async_rest_client::async_rest_client::create(ioc) };

  auto connect_future { boost::asio::co_spawn(
    ioc,
    [client]() -> boost::asio::awaitable<void> {
      try
        {
          co_await client->connect("httpbin.org/get");
          ADD_FAILURE() << "Expected exception for URL without scheme";
        }
      catch (const std::invalid_argument &e)
        {
          EXPECT_STREQ(
            e.what(),
            "URL must include scheme (http:// or https://)");
        }
    },
    boost::asio::use_future) };

  ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}

TEST(AsyncRestClientTest, ConnectRejectsUnsupportedScheme)
{
  boost::asio::io_context ioc {};
  auto client { async_rest_client::async_rest_client::create(ioc) };

  auto connect_future { boost::asio::co_spawn(
    ioc,
    [client]() -> boost::asio::awaitable<void> {
      try
        {
          co_await client->connect("ftp://example.com");
          ADD_FAILURE() << "Expected exception for unsupported scheme";
        }
      catch (const std::invalid_argument &e)
        {
          EXPECT_STREQ(e.what(), "Only http and https schemes are supported");
        }
    },
    boost::asio::use_future) };

  ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}

TEST(AsyncRestClientTest, ConnectToRealEndpointSucceeds)
{
  boost::asio::io_context ioc {};
  auto client { async_rest_client::async_rest_client::create(ioc) };

  auto connect_future { boost::asio::co_spawn(
    ioc,
    std::bind_front(
      &async_rest_client::async_rest_client::connect,
      client,
      "https://httpbin.org/get"),
    boost::asio::use_future) };

  ioc.run();

  bool result { false };
  EXPECT_NO_THROW(result = connect_future.get());
  EXPECT_TRUE(result);
}

TEST(AsyncRestClientTest, ConcurrentConnectCalls)
{
  boost::asio::io_context ioc {};
  auto client { async_rest_client::async_rest_client::create(ioc) };

  auto connect_future1 { boost::asio::co_spawn(
    ioc,
    std::bind_front(
      &async_rest_client::async_rest_client::connect,
      client,
      "https://httpbin.org"),
    boost::asio::use_future) };

  auto connect_future2 { boost::asio::co_spawn(
    ioc,
    std::bind_front(
      &async_rest_client::async_rest_client::connect,
      client,
      "https://httpbin.org"),
    boost::asio::use_future) };

  ioc.run();

  bool result1 { false };
  bool result2 { false };

  EXPECT_NO_THROW(result1 = connect_future1.get());
  EXPECT_NO_THROW(result2 = connect_future2.get());

  // Exactly one should succeed, one should fail
  // because they should interleave rather than occurring serially
  EXPECT_TRUE((result1 && !result2) || (!result1 && result2));
}

TEST(AsyncRestClientTest, SequentialConnectionsToDifferentHosts)
{
  boost::asio::io_context ioc {};
  auto client { async_rest_client::async_rest_client::create(ioc) };

  auto connect_future { boost::asio::co_spawn(
    ioc,
    [client]() -> boost::asio::awaitable<void> {
      const bool result1 = co_await client->connect("https://httpbin.org");
      EXPECT_TRUE(result1);

      const bool result2 = co_await client->connect("https://google.com");
      EXPECT_TRUE(result2);
    },
    boost::asio::use_future) };

  ioc.run();
  EXPECT_NO_THROW(connect_future.get());
}
