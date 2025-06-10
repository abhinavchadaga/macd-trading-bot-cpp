#include <gtest/gtest.h>
#include "async_rest_client/async_rest_client.hpp"
#include "LoggingUtils.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class AsyncRestClientRequestResponseTest : public ::testing::Test {
protected:
    void SetUp() override {
        configure_logging();
    }

    boost::asio::io_context _ioc;
    boost::asio::ssl::context _ssl_ctx{boost::asio::ssl::context::tlsv12_client};
};

TEST_F(AsyncRestClientRequestResponseTest, RequestBuilderEmptyBody) {
    using namespace async_rest_client;
    using namespace boost::beast;
    
    http::fields headers{};
    headers.set(http::field::authorization, "Bearer token123");
    
    auto writer = request_builder<http::empty_body>::build(
        "https://httpbin.org/get",
        headers,
        "test-client/1.0"
    );
    
    http::request<http::empty_body> req{};
    writer(req);
    
    EXPECT_EQ(req.method(), http::verb::get);
    EXPECT_EQ(req.target(), "/get");
    EXPECT_EQ(req.version(), 11);
    EXPECT_EQ(req[http::field::host], "httpbin.org");
    EXPECT_EQ(req[http::field::user_agent], "test-client/1.0");
    EXPECT_EQ(req[http::field::authorization], "Bearer token123");
    EXPECT_EQ(req[http::field::connection], "close");
}

TEST_F(AsyncRestClientRequestResponseTest, RequestBuilderStringBody) {
    using namespace async_rest_client;
    using namespace boost::beast;
    
    http::fields headers{};
    headers.set(http::field::authorization, "Bearer token123");
    
    std::string body_content = R"({"test": "data"})";
    
    auto writer = request_builder<http::string_body>::build(
        "https://httpbin.org/post",
        headers,
        "test-client/1.0",
        body_content
    );
    
    http::request<http::string_body> req{};
    writer(req);
    
    EXPECT_EQ(req.method(), http::verb::post);
    EXPECT_EQ(req.target(), "/post");
    EXPECT_EQ(req.version(), 11);
    EXPECT_EQ(req[http::field::host], "httpbin.org");
    EXPECT_EQ(req[http::field::user_agent], "test-client/1.0");
    EXPECT_EQ(req[http::field::authorization], "Bearer token123");
    EXPECT_EQ(req[http::field::content_type], "application/json");
    EXPECT_EQ(req[http::field::connection], "close");
    EXPECT_EQ(req.body(), body_content);
}

TEST_F(AsyncRestClientRequestResponseTest, ResponseParserStringBodySuccess) {
    using namespace async_rest_client;
    using namespace boost::beast;
    
    auto reader = response_parser<http::string_body>::create_reader();
    
    http::response<http::string_body> res{};
    res.result(http::status::ok);
    res.body() = R"({"result": "success"})";
    
    bool callback_called{false};
    boost::system::error_code result_ec;
    std::string result_body;
    
    auto completion_handler = [&](boost::system::error_code ec, const std::string& body) {
        callback_called = true;
        result_ec = ec;
        result_body = body;
    };
    
    reader(res, boost::system::error_code{}, completion_handler);
    
    EXPECT_TRUE(callback_called);
    EXPECT_FALSE(result_ec);
    EXPECT_EQ(result_body, R"({"result": "success"})");
}

TEST_F(AsyncRestClientRequestResponseTest, ResponseParserStringBodyError) {
    using namespace async_rest_client;
    using namespace boost::beast;
    
    auto reader = response_parser<http::string_body>::create_reader();
    
    http::response<http::string_body> res{};
    res.result(http::status::not_found);
    res.body() = "Not Found";
    
    bool callback_called{false};
    boost::system::error_code result_ec;
    std::string result_body;
    
    auto completion_handler = [&](boost::system::error_code ec, const std::string& body) {
        callback_called = true;
        result_ec = ec;
        result_body = body;
    };
    
    reader(res, boost::system::error_code{}, completion_handler);
    
    EXPECT_TRUE(callback_called);
    EXPECT_TRUE(result_ec);
    EXPECT_EQ(result_ec.value(), 404);
    EXPECT_EQ(result_body, "Not Found");
}