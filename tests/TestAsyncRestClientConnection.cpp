#include <gtest/gtest.h>
#include "async_rest_client/async_rest_client.hpp"
#include "LoggingUtils.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class AsyncRestClientConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        configure_logging();
        _ssl_ctx.set_default_verify_paths();
        _ssl_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
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

TEST_F(AsyncRestClientConnectionTest, CreateClient) {
    _client = async_rest_client::async_rest_client::create(_ioc, _ssl_ctx);
    ASSERT_NE(_client, nullptr);
    EXPECT_FALSE(_client->is_connected());
}

TEST_F(AsyncRestClientConnectionTest, ConnectToHttpBin) {
    _client = async_rest_client::async_rest_client::create(_ioc, _ssl_ctx);
    
    bool callback_called{false};
    boost::system::error_code result_ec;
    std::string result_message;

    _client->connect("httpbin.org", "443", [&](boost::system::error_code ec, const std::string& message) {
        callback_called = true;
        result_ec = ec;
        result_message = message;
    });

    _ioc.run();

    EXPECT_TRUE(callback_called);
    EXPECT_FALSE(result_ec) << "Connection failed: " << result_ec.message();
    EXPECT_TRUE(_client->is_connected());
    EXPECT_EQ(result_message, "Connected");
}

TEST_F(AsyncRestClientConnectionTest, DisconnectClient) {
    _client = async_rest_client::async_rest_client::create(_ioc, _ssl_ctx);
    
    bool callback_called{false};

    _client->connect("httpbin.org", "443", [&](boost::system::error_code ec, const std::string& message) {
        callback_called = true;
        EXPECT_FALSE(ec);
        EXPECT_TRUE(_client->is_connected());
        
        _client->disconnect();
        EXPECT_FALSE(_client->is_connected());
    });

    _ioc.run();
    EXPECT_TRUE(callback_called);
}

TEST_F(AsyncRestClientConnectionTest, ConnectToInvalidHost) {
    _client = async_rest_client::async_rest_client::create(_ioc, _ssl_ctx);
    
    bool callback_called{false};
    boost::system::error_code result_ec;

    _client->connect("invalid-host-does-not-exist.com", "443", [&](boost::system::error_code ec, const std::string& message) {
        callback_called = true;
        result_ec = ec;
    });

    _ioc.run();

    EXPECT_TRUE(callback_called);
    EXPECT_TRUE(result_ec);
    EXPECT_FALSE(_client->is_connected());
}