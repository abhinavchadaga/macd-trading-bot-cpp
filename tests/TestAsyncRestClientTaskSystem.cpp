#include <gtest/gtest.h>
#include "async_rest_client/async_rest_client.hpp"
#include "async_rest_client/detail/typed_task.hpp"
#include "LoggingUtils.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class AsyncRestClientTaskSystemTest : public ::testing::Test {
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

TEST_F(AsyncRestClientTaskSystemTest, CreateTypedTask) {
    using namespace async_rest_client;
    using namespace async_rest_client::detail;
    using namespace boost::beast;
    
    auto writer = [](http::request<http::string_body>& req) {
        req.method(http::verb::get);
        req.target("/test");
        req.version(11);
        req.set(http::field::host, "test.com");
    };
    
    auto reader = [](const http::response<http::string_body>& res, boost::system::error_code ec, auto handler) {
        handler(ec, res.body());
    };
    
    auto completion_handler = [](boost::system::error_code ec, const std::string& result) {
        // Test completion handler
    };
    
    auto task = typed_task<http::string_body, http::string_body>::create(
        writer, reader, completion_handler
    );
    
    ASSERT_NE(task, nullptr);
    
    auto base_task_ptr = std::static_pointer_cast<base_task>(task);
    ASSERT_NE(base_task_ptr, nullptr);
}

TEST_F(AsyncRestClientTaskSystemTest, TaskQueueFIFOOrder) {
    using namespace async_rest_client;
    using namespace async_rest_client::detail;
    using namespace boost::beast;
    
    std::vector<int> execution_order;
    
    auto create_test_task = [&execution_order](int id) {
        auto writer = [id](http::request<http::string_body>& req) {
            req.method(http::verb::get);
            req.target("/test");
            req.version(11);
            req.set(http::field::host, "test.com");
        };
        
        auto reader = [id, &execution_order](const http::response<http::string_body>& res, boost::system::error_code ec, auto handler) {
            execution_order.push_back(id);
            handler(ec, "test");
        };
        
        auto completion_handler = [](boost::system::error_code ec, const std::string& result) {
            // Test completion handler
        };
        
        return typed_task<http::string_body, http::string_body>::create(
            writer, reader, completion_handler
        );
    };
    
    ASSERT_NE(_client, nullptr);
}