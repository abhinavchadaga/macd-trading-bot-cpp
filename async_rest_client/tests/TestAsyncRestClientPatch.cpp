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

class AsyncRestClientPatchTest : public ::testing::Test
{
protected:
    void SetUp() override { _client = RestClient::create(_ioc); }

    void TearDown() override { _client.reset(); }

    boost::asio::io_context     _ioc{};
    std::shared_ptr<RestClient> _client;
};

TEST_F(AsyncRestClientPatchTest, BasicPatchRequest)
{
    auto future = net::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");

            nlohmann::json json_body{{"operation", "update"}, {"field", "value"}};
            std::string    body_str = json_body.dump();

            auto [ec, response] =
                co_await _client->request<http::verb::patch>("https://httpbin.org/patch", headers, body_str);

            EXPECT_FALSE(ec) << "PATCH request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);
            EXPECT_FALSE(response.body().empty());

            const auto json_response = nlohmann::json::parse(response.body());
            EXPECT_TRUE(json_response.is_object());
            EXPECT_TRUE(json_response.contains("json"));
            EXPECT_EQ(json_response["json"]["operation"], "update");
            EXPECT_EQ(json_response["json"]["field"], "value");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPatchTest, PatchWithCustomHeaders)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            headers.set(http::field::user_agent, "TestAgent/1.0");
            headers.insert("X-Patch-Header", "PatchTestValue");

            nlohmann::json json_body{{"patch_test", "headers"}, {"id", 456}};
            std::string    body_str = json_body.dump();

            auto [ec, response] =
                co_await _client->request<http::verb::patch>("https://httpbin.org/patch", headers, body_str);

            EXPECT_FALSE(ec) << "PATCH request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::ok);

            const auto  json_response = nlohmann::json::parse(response.body());
            const auto& headers_obj   = json_response["headers"];
            EXPECT_EQ(headers_obj["User-Agent"], "TestAgent/1.0");
            EXPECT_EQ(headers_obj["X-Patch-Header"], "PatchTestValue");
            EXPECT_EQ(headers_obj["Content-Type"], "application/json");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPatchTest, Patch404Response)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            std::string body = R"({"test": "404"})";

            auto [ec, response] =
                co_await _client->request<http::verb::patch>("https://httpbin.org/status/404", headers, body);

            EXPECT_FALSE(ec) << "PATCH request failed: " << ec.message();
            EXPECT_EQ(response.result(), http::status::not_found);
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientPatchTest, PatchInvalidHost)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            std::string body = R"({"test": "invalid_host"})";

            auto [ec, response] = co_await _client->request<http::verb::patch>(
                "https://this-host-does-not-exist-12345.com/patch", headers, body);

            EXPECT_TRUE(ec) << "Expected error for invalid host but got success";
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}
