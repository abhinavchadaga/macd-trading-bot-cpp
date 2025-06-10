#include <gtest/gtest.h>
#include "async_rest_client/async_rest_client.hpp"
#include "LoggingUtils.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>

class AsyncRestClientGetTest : public ::testing::Test {
protected:
    void SetUp() override {
        configure_logging();
        _ssl_ctx.set_default_verify_paths();
        _ssl_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
        _client = async_rest_client::async_rest_client::create(_ioc, _ssl_ctx);
    }

    void TearDown() override {
        if (_client && _client->is_connected()) {
            _client->disconnect();
        }
    }

    boost::asio::io_context _ioc;
    boost::asio::ssl::context _ssl_ctx{boost::asio::ssl::context::tlsv12_client};
    std::shared_ptr<async_rest_client::async_rest_client> _client;
};

TEST_F(AsyncRestClientGetTest, GetRequestToHttpBin) {
    using namespace boost::beast;
    
    bool connect_callback_called{false};
    bool get_callback_called{false};
    boost::system::error_code get_result_ec;
    std::string get_result_body;

    _client->connect("httpbin.org", "443", [&](boost::system::error_code ec, const std::string& message) {
        connect_callback_called = true;
        EXPECT_FALSE(ec) << "Connection failed: " << ec.message();
        EXPECT_TRUE(_client->is_connected());

        if (!ec) {
            http::fields headers{};
            headers.set(http::field::accept, "application/json");

            _client->get<http::string_body>(
                "https://httpbin.org/get",
                headers,
                [&](boost::system::error_code ec, const std::string& result) {
                    get_callback_called = true;
                    get_result_ec = ec;
                    get_result_body = result;
                }
            );
        }
    });

    _ioc.run();

    EXPECT_TRUE(connect_callback_called);
    EXPECT_TRUE(get_callback_called);
    EXPECT_FALSE(get_result_ec) << "GET request failed: " << get_result_ec.message();
    EXPECT_FALSE(get_result_body.empty());

    auto json_response = nlohmann::json::parse(get_result_body, nullptr, false);
    EXPECT_FALSE(json_response.is_discarded());
    EXPECT_TRUE(json_response.contains("url"));
    EXPECT_EQ(json_response["url"], "https://httpbin.org/get");
}

TEST_F(AsyncRestClientGetTest, GetRequestWithCustomHeaders) {
    using namespace boost::beast;
    
    bool connect_callback_called{false};
    bool get_callback_called{false};
    boost::system::error_code get_result_ec;
    std::string get_result_body;

    _client->connect("httpbin.org", "443", [&](boost::system::error_code ec, const std::string& message) {
        connect_callback_called = true;
        EXPECT_FALSE(ec);

        if (!ec) {
            http::fields headers{};
            headers.set(http::field::accept, "application/json");
            headers.insert("X-Custom-Header", "TestValue");

            _client->get<http::string_body>(
                "https://httpbin.org/get",
                headers,
                [&](boost::system::error_code ec, const std::string& result) {
                    get_callback_called = true;
                    get_result_ec = ec;
                    get_result_body = result;
                }
            );
        }
    });

    _ioc.run();

    EXPECT_TRUE(connect_callback_called);
    EXPECT_TRUE(get_callback_called);
    EXPECT_FALSE(get_result_ec);

    auto json_response = nlohmann::json::parse(get_result_body, nullptr, false);
    EXPECT_FALSE(json_response.is_discarded());
    EXPECT_TRUE(json_response.contains("headers"));
    EXPECT_TRUE(json_response["headers"].contains("X-Custom-Header"));
    EXPECT_EQ(json_response["headers"]["X-Custom-Header"], "TestValue");
}