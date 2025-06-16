#include "async_rest_client/async_rest_client.hpp"
#include "my_logger.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>

namespace net    = boost::asio;
namespace http   = boost::beast::http;
using RestClient = async_rest_client::async_rest_client;

class AsyncRestClientPostTest : public ::testing::Test
{
protected:
    void SetUp() override { _client = RestClient::create(_ioc); }

    void TearDown() override { _client.reset(); }

    boost::asio::io_context     _ioc{};
    std::shared_ptr<RestClient> _client;
};

TEST_F(AsyncRestClientPostTest, BasicPostRequest)
{
    auto future = net::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");

            nlohmann::json json_body{{"key", "value"}, {"number", 42}};
            std::string    body_str = json_body.dump();

            auto [ec, response] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/post", headers, body_str);

            EXPECT_FALSE(ec) << "POST request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);
            EXPECT_FALSE(response.body().empty());

            const auto json_response = nlohmann::json::parse(response.body());
            EXPECT_TRUE(json_response.is_object());
            EXPECT_TRUE(json_response.contains("json"));
            EXPECT_EQ(json_response["json"]["key"], "value");
            EXPECT_EQ(json_response["json"]["number"], 42);
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPostTest, PostWithCustomHeaders)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            headers.set(http::field::user_agent, "TestAgent/1.0");
            headers.insert("X-Custom-Header", "PostTestValue");

            nlohmann::json json_body{{"test", "post_headers"}, {"value", 123}};
            std::string    body_str = json_body.dump();

            auto [ec, response] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/post", headers, body_str);

            EXPECT_FALSE(ec) << "POST request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);

            const auto  json_response = nlohmann::json::parse(response.body());
            const auto& headers_obj   = json_response["headers"];
            EXPECT_EQ(headers_obj["User-Agent"], "TestAgent/1.0");
            EXPECT_EQ(headers_obj["X-Custom-Header"], "PostTestValue");
            EXPECT_EQ(headers_obj["Content-Type"], "application/json");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPostTest, PostLargePayload)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");

            nlohmann::json json_body{};
            for (int i = 0; i < 100; ++i)
            {
                json_body["item_" + std::to_string(i)] = "value_" + std::to_string(i);
            }
            std::string body_str = json_body.dump();

            auto [ec, response] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/post", headers, body_str);

            EXPECT_FALSE(ec) << "POST request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);

            const auto json_response = nlohmann::json::parse(response.body());
            EXPECT_TRUE(json_response.contains("json"));
            EXPECT_EQ(json_response["json"].size(), 100u);
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPostTest, Post404Response)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            std::string body = R"({"test": "404"})";

            auto [ec, response] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/status/404", headers, body);

            EXPECT_FALSE(ec) << "POST request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::not_found);
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPostTest, PostInvalidHost)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            std::string body = R"({"test": "invalid_host"})";

            auto [ec, response] = co_await _client->post<http::string_body, http::string_body>(
                "https://this-host-does-not-exist-12345.com/post", headers, body);

            EXPECT_TRUE(ec) << "Expected error for invalid host but got success";
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}
