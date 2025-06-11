#include "async_rest_client/async_rest_client.hpp"
#include "async_rest_client/typed_task.hpp"

#include <gtest/gtest.h>

TEST(AsyncRestClientTest, CanCreateClient)
{
  boost::asio::io_context ioc {};
  auto client = async_rest_client::async_rest_client::create(ioc);
  ASSERT_NE(client, nullptr);
}

TEST(AsyncRestClientTest, CanInstantiateConnectionContext)
{
  boost::asio::io_context               ioc {};
  async_rest_client::beast::tcp_stream  tcp_stream { ioc };
  async_rest_client::beast::flat_buffer buffer {};

  async_rest_client::http_connection_context http_ctx { tcp_stream, buffer };
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
