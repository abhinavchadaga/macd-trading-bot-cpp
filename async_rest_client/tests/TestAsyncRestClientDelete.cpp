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

class AsyncRestClientDeleteTest : public ::testing::Test
{
protected:
    void SetUp() override { _client = RestClient::create(_ioc); }

    void TearDown() override { _client.reset(); }

    boost::asio::io_context     _ioc{};
    std::shared_ptr<RestClient> _client;
};

TEST_F(AsyncRestClientDeleteTest, BasicDeleteRequest)
{
    auto future = net::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            auto [ec, response] = co_await _client->delete_<http::string_body>("https://httpbin.org/delete");

            EXPECT_FALSE(ec) << "DELETE request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);
            EXPECT_FALSE(response.body().empty());

            const auto json_response = nlohmann::json::parse(response.body());
            EXPECT_TRUE(json_response.is_object());
            EXPECT_TRUE(json_response.contains("url"));
            EXPECT_EQ(json_response["url"], "https://httpbin.org/delete");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientDeleteTest, DeleteWithCustomHeaders)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::user_agent, "TestAgent/1.0");
            headers.insert("X-Delete-Header", "DeleteTestValue");

            auto [ec, response] = co_await _client->delete_<http::string_body>("https://httpbin.org/delete", headers);

            EXPECT_FALSE(ec) << "DELETE request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);

            const auto  json_response = nlohmann::json::parse(response.body());
            const auto& headers_obj   = json_response["headers"];
            EXPECT_EQ(headers_obj["User-Agent"], "TestAgent/1.0");
            EXPECT_EQ(headers_obj["X-Delete-Header"], "DeleteTestValue");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientDeleteTest, DeleteWithQueryParameters)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            auto [ec, response] =
                co_await _client->delete_<http::string_body>("https://httpbin.org/delete?resource=123&force=true");

            EXPECT_FALSE(ec) << "DELETE request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);

            const auto  json_response = nlohmann::json::parse(response.body());
            const auto& args_obj      = json_response["args"];

            EXPECT_EQ(args_obj["resource"], "123");
            EXPECT_EQ(args_obj["force"], "true");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientDeleteTest, Delete404Response)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            auto [ec, response] = co_await _client->delete_<http::string_body>("https://httpbin.org/status/404");

            EXPECT_FALSE(ec) << "DELETE request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::not_found);
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientDeleteTest, DeleteInvalidHost)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            auto [ec, response] =
                co_await _client->delete_<http::string_body>("https://this-host-does-not-exist-12345.com/delete");

            EXPECT_TRUE(ec) << "Expected error for invalid host but got success";
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}
