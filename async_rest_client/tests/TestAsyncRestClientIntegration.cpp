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

class AsyncRestClientIntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override { _client = RestClient::create(_ioc); }

    void TearDown() override { _client.reset(); }

    boost::asio::io_context     _ioc{};
    std::shared_ptr<RestClient> _client;
};

TEST_F(AsyncRestClientIntegrationTest, SequentialAllVerbsRequests)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");

            // GET request
            auto [ec1, response1] = co_await _client->get<http::string_body>("https://httpbin.org/get?sequential=1");

            EXPECT_FALSE(ec1) << "GET request failed: " << ec1.message();
            EXPECT_EQ(response1.result(), http::status::ok);

            const auto json1 = nlohmann::json::parse(response1.body());
            EXPECT_EQ(json1["args"]["sequential"], "1");

            // POST request
            nlohmann::json post_body{{"action", "create"}, {"id", 1}};
            std::string    post_str = post_body.dump();

            auto [ec2, response2] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/post", headers, post_str);

            EXPECT_FALSE(ec2) << "POST request failed: " << ec2.message();
            EXPECT_EQ(response2.result(), http::status::ok);

            const auto json2 = nlohmann::json::parse(response2.body());
            EXPECT_EQ(json2["json"]["action"], "create");

            // DELETE request
            auto [ec3, response3] = co_await _client->delete_<http::string_body>("https://httpbin.org/delete?id=1");

            EXPECT_FALSE(ec3) << "DELETE request failed: " << ec3.message();
            EXPECT_EQ(response3.result(), http::status::ok);

            const auto json3 = nlohmann::json::parse(response3.body());
            EXPECT_EQ(json3["args"]["id"], "1");

            // PATCH request
            nlohmann::json patch_body{{"action", "partial_update"}, {"id", 1}};
            std::string    patch_str = patch_body.dump();

            auto [ec4, response4] = co_await _client->patch<http::string_body, http::string_body>(
                "https://httpbin.org/patch", headers, patch_str);

            EXPECT_FALSE(ec4) << "PATCH request failed: " << ec4.message();
            EXPECT_EQ(response4.result(), http::status::ok);

            const auto json4 = nlohmann::json::parse(response4.body());
            EXPECT_EQ(json4["json"]["action"], "partial_update");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientIntegrationTest, ConcurrentAllVerbsRequests)
{
    auto future = net::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");

            nlohmann::json post_body{{"concurrent", "post"}};
            nlohmann::json patch_body{{"concurrent", "patch"}};
            std::string    post_str  = post_body.dump();
            std::string    patch_str = patch_body.dump();

            // Create all awaitables first (queues them concurrently)
            auto get_awaitable = _client->get<http::string_body>("https://httpbin.org/get?concurrent=get");
            auto post_awaitable =
                _client->post<http::string_body, http::string_body>("https://httpbin.org/post", headers, post_str);
            auto delete_awaitable = _client->delete_<http::string_body>("https://httpbin.org/delete?concurrent=delete");
            auto patch_awaitable =
                _client->patch<http::string_body, http::string_body>("https://httpbin.org/patch", headers, patch_str);

            // Then await them (they execute concurrently)
            auto [ec1, response1] = co_await std::move(get_awaitable);
            auto [ec2, response2] = co_await std::move(post_awaitable);
            auto [ec3, response3] = co_await std::move(delete_awaitable);
            auto [ec4, response4] = co_await std::move(patch_awaitable);

            EXPECT_FALSE(ec1) << "Concurrent GET failed: " << ec1.message();
            EXPECT_FALSE(ec2) << "Concurrent POST failed: " << ec2.message();
            EXPECT_FALSE(ec3) << "Concurrent DELETE failed: " << ec3.message();
            EXPECT_FALSE(ec4) << "Concurrent PATCH failed: " << ec4.message();

            EXPECT_EQ(response1.result(), http::status::ok);
            EXPECT_EQ(response2.result(), http::status::ok);
            EXPECT_EQ(response3.result(), http::status::ok);
            EXPECT_EQ(response4.result(), http::status::ok);

            const auto json1 = nlohmann::json::parse(response1.body());
            const auto json2 = nlohmann::json::parse(response2.body());
            const auto json3 = nlohmann::json::parse(response3.body());
            const auto json4 = nlohmann::json::parse(response4.body());

            EXPECT_EQ(json1["args"]["concurrent"], "get");
            EXPECT_EQ(json2["json"]["concurrent"], "post");
            EXPECT_EQ(json3["args"]["concurrent"], "delete");
            EXPECT_EQ(json4["json"]["concurrent"], "patch");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientIntegrationTest, MixedHostRequests)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");

            nlohmann::json body{{"mixed_host", "test"}};
            std::string    body_str = body.dump();

            // Request to httpbin.org
            auto [ec1, response1] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/post", headers, body_str);

            EXPECT_FALSE(ec1) << "First mixed host POST failed: " << ec1.message();
            EXPECT_EQ(response1.result(), http::status::ok);

            // Request to different host (postman-echo.com)
            auto [ec2, response2] = co_await _client->delete_<http::string_body>("https://postman-echo.com/delete");

            EXPECT_FALSE(ec2) << "Second mixed host DELETE failed: " << ec2.message();
            EXPECT_EQ(response2.result(), http::status::ok);

            // Back to httpbin.org
            auto [ec3, response3] = co_await _client->get<http::string_body>("https://httpbin.org/get");

            EXPECT_FALSE(ec3) << "Third mixed host GET failed: " << ec3.message();
            EXPECT_EQ(response3.result(), http::status::ok);

            const auto json1 = nlohmann::json::parse(response1.body());
            const auto json2 = nlohmann::json::parse(response2.body());
            const auto json3 = nlohmann::json::parse(response3.body());

            EXPECT_EQ(json1["url"], "https://httpbin.org/post");
            EXPECT_EQ(json2["url"], "https://postman-echo.com/delete");
            EXPECT_EQ(json3["url"], "https://httpbin.org/get");
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientIntegrationTest, AllVerbsErrorHandling404)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            std::string body = R"({"test": "404"})";

            auto [ec1, response1] = co_await _client->get<http::string_body>("https://httpbin.org/status/404");
            EXPECT_FALSE(ec1) << "GET request failed: " << ec1.message();
            EXPECT_EQ(response1.result(), http::status::not_found);

            auto [ec2, response2] = co_await _client->post<http::string_body, http::string_body>(
                "https://httpbin.org/status/404", headers, body);
            EXPECT_FALSE(ec2) << "POST request failed: " << ec2.message();
            EXPECT_EQ(response2.result(), http::status::not_found);

            auto [ec3, response3] = co_await _client->delete_<http::string_body>("https://httpbin.org/status/404");
            EXPECT_FALSE(ec3) << "DELETE request failed: " << ec3.message();
            EXPECT_EQ(response3.result(), http::status::not_found);

            auto [ec4, response4] = co_await _client->patch<http::string_body, http::string_body>(
                "https://httpbin.org/status/404", headers, body);
            EXPECT_FALSE(ec4) << "PATCH request failed: " << ec4.message();
            EXPECT_EQ(response4.result(), http::status::not_found);
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

TEST_F(AsyncRestClientIntegrationTest, AllVerbsErrorHandlingInvalidHost)
{
    auto future = boost::asio::co_spawn(
        _ioc,
        [this]() -> boost::asio::awaitable<void>
        {
            http::fields headers{};
            headers.set(http::field::content_type, "application/json");
            std::string body = R"({"test": "invalid_host"})";

            auto [ec1, response1] =
                co_await _client->get<http::string_body>("https://this-host-does-not-exist-12345.com/get");
            EXPECT_TRUE(ec1) << "Expected error for invalid host but got success";

            auto [ec2, response2] = co_await _client->post<http::string_body, http::string_body>(
                "https://this-host-does-not-exist-12345.com/post", headers, body);
            EXPECT_TRUE(ec2) << "Expected error for invalid host but got success";

            auto [ec3, response3] =
                co_await _client->delete_<http::string_body>("https://this-host-does-not-exist-12345.com/delete");
            EXPECT_TRUE(ec3) << "Expected error for invalid host but got success";

            auto [ec4, response4] = co_await _client->patch<http::string_body, http::string_body>(
                "https://this-host-does-not-exist-12345.com/patch", headers, body);
            EXPECT_TRUE(ec4) << "Expected error for invalid host but got success";
        },
        boost::asio::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}
