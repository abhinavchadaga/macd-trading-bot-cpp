#include "async_rest_client/async_rest_client.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>

class AsyncRestClientGetTest : public ::testing::Test
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

  boost::asio::io_context                               _ioc {};
  std::shared_ptr<async_rest_client::async_rest_client> _client;
};

TEST_F(AsyncRestClientGetTest, BasicGetRequest)
{
  auto future = async_rest_client::net::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get");

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::ok);
      EXPECT_FALSE(response.body().empty());

      const auto json_response = nlohmann::json::parse(response.body());
      EXPECT_TRUE(json_response.is_object());
      EXPECT_TRUE(json_response.contains("url"));
      EXPECT_EQ(json_response["url"], "https://httpbin.org/get");
    },
    boost::asio::use_future);

  _ioc.run();
  EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientGetTest, GetWithCustomHeaders)
{
  auto future = boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      async_rest_client::http::fields headers {};
      headers.set(async_rest_client::http::field::user_agent, "TestAgent/1.0");
      headers.insert("X-Custom-Header", "TestValue");

      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get",
          headers);

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::ok);

      const auto  json_response = nlohmann::json::parse(response.body());
      const auto &headers_obj   = json_response["headers"];
      EXPECT_EQ(headers_obj["User-Agent"], "TestAgent/1.0");
      EXPECT_EQ(headers_obj["X-Custom-Header"], "TestValue");
    },
    boost::asio::use_future);

  _ioc.run();
  EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientGetTest, GetWithQueryParameters)
{
  auto future = boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get?param1=value1&param2=value2");

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::ok);

      const auto  json_response = nlohmann::json::parse(response.body());
      const auto &args_obj      = json_response["args"];

      EXPECT_EQ(args_obj["param1"], "value1");
      EXPECT_EQ(args_obj["param2"], "value2");
    },
    boost::asio::use_future);

  _ioc.run();
  EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientGetTest, SequentialGetRequests)
{
  auto future = boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec1, response1]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get?request=1");

      EXPECT_FALSE(ec1) << "First GET request failed: " << ec1.message();
      EXPECT_EQ(response1.result(), async_rest_client::http::status::ok);

      const auto  json1 = nlohmann::json::parse(response1.body());
      const auto &args1 = json1["args"];
      EXPECT_EQ(args1["request"], "1");


      auto [ec2, response2]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get?request=2");

      EXPECT_FALSE(ec2) << "Second GET request failed: " << ec2.message();
      EXPECT_EQ(response2.result(), async_rest_client::http::status::ok);

      const auto  json2 = nlohmann::json::parse(response2.body());
      const auto &args2 = json2["args"];
      EXPECT_EQ(args2["request"], "2");
    },
    boost::asio::use_future);

  _ioc.run();
  EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientGetTest, ConcurrentGetRequests)
{
  auto future = async_rest_client::net::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto awaitable1 = _client->get<async_rest_client::http::string_body>(
        "https://httpbin.org/get?concurrent=1");
      auto awaitable2 = _client->get<async_rest_client::http::string_body>(
        "https://httpbin.org/get?concurrent=2");

      auto [ec1, response1] = co_await std::move(awaitable1);
      auto [ec2, response2] = co_await std::move(awaitable2);

      EXPECT_FALSE(ec1) << "First concurrent GET failed: "
                        << ec1.message() + " " + ec1.location().to_string();
      EXPECT_FALSE(ec2) << "Second concurrent GET failed: " << ec2.message();

      EXPECT_EQ(response1.result(), async_rest_client::http::status::ok);
      EXPECT_EQ(response2.result(), async_rest_client::http::status::ok);

      const auto &json1 { nlohmann::json::parse(response1.body()) };
      const auto &json2 { nlohmann::json::parse(response2.body()) };

      const auto &args1 { json1["args"] };
      const auto &args2 { json2["args"] };

      EXPECT_EQ(args1["concurrent"], "1");
      EXPECT_EQ(args2["concurrent"], "2");
    },
    boost::asio::use_future);

  _ioc.run();
  EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientGetTest, MixedHostRequests)
{
  auto future = boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec1, response1]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get");

      EXPECT_FALSE(ec1) << "First mixed host GET failed: " << ec1.message();
      EXPECT_EQ(response1.result(), async_rest_client::http::status::ok);

      const auto json1 = nlohmann::json::parse(response1.body());
      EXPECT_EQ(json1["url"], "https://httpbin.org/get");


      auto [ec2, response2]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://postman-echo.com/get");

      EXPECT_FALSE(ec2) << "Second mixed host GET failed: " << ec2.message();
      EXPECT_EQ(response2.result(), async_rest_client::http::status::ok);

      const auto json2 = nlohmann::json::parse(response2.body());
      EXPECT_EQ(json2["url"], "https://postman-echo.com/get");
    },
    boost::asio::use_future);

  _ioc.run();
  EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientGetTest, HttpAndHttpsRequests)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec_https, https_response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get");

      EXPECT_FALSE(ec_https) << "HTTPS GET failed: " << ec_https.message();
      EXPECT_EQ(https_response.result(), async_rest_client::http::status::ok);

      auto [ec_http, http_response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "http://httpbin.org/get");

      EXPECT_FALSE(ec_http) << "HTTP GET failed: " << ec_http.message();
      EXPECT_EQ(http_response.result(), async_rest_client::http::status::ok);


      auto [ec_https2, https_response2]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get");
      EXPECT_FALSE(ec_https2)
        << "Second HTTPS GET failed: " << ec_https2.message();
      EXPECT_EQ(https_response2.result(), async_rest_client::http::status::ok);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}

TEST_F(AsyncRestClientGetTest, GetInvalidUrl)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://this-host-does-not-exist-12345.com/get");

      EXPECT_TRUE(ec) << "Expected error for invalid host but got success";
      EXPECT_FALSE(ec.message().empty());
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}

TEST_F(AsyncRestClientGetTest, Get404Response)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/status/404");

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::not_found);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}

TEST_F(AsyncRestClientGetTest, GetRedirectResponse)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/redirect/1");

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::found);
      EXPECT_TRUE(
        response.count(async_rest_client::http::field::location) > 0);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}

TEST_F(AsyncRestClientGetTest, GetLargeResponse)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/bytes/10000");

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::ok);
      EXPECT_EQ(response.body().size(), 10000u);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}

TEST_F(AsyncRestClientGetTest, ResponseHeadersArePresent)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get");

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::ok);

      EXPECT_TRUE(
        response.count(async_rest_client::http::field::content_type) > 0);
      EXPECT_TRUE(
        response.count(async_rest_client::http::field::content_length) > 0);
      EXPECT_TRUE(response.count(async_rest_client::http::field::server) > 0);

      const auto content_type
        = response[async_rest_client::http::field::content_type];
      EXPECT_TRUE(content_type.find("application/json") != std::string::npos);
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}

TEST_F(AsyncRestClientGetTest, GetWithNoHeaders)
{
  auto get_future { boost::asio::co_spawn(
    _ioc,
    [this]() -> boost::asio::awaitable<void> {
      auto [ec, response]
        = co_await _client->get<async_rest_client::http::string_body>(
          "https://httpbin.org/get",
          {});

      EXPECT_FALSE(ec) << "GET request failed: " << ec.message();
      EXPECT_EQ(response.result(), async_rest_client::http::status::ok);

      const auto json_response = nlohmann::json::parse(response.body());
      const auto &headers_obj   = json_response["headers"];

      EXPECT_TRUE(headers_obj.contains("Host"));
      EXPECT_TRUE(headers_obj.contains("User-Agent"));
    },
    boost::asio::use_future) };

  _ioc.run();
  EXPECT_NO_THROW(get_future.get());
}
