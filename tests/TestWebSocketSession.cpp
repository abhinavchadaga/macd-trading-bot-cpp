#include "WebSocketSession.hpp"

#include <boost/asio.hpp>
#include <chrono>
#include <filesystem>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

namespace asio = boost::asio;
namespace ssl  = asio::ssl;
namespace fs   = std::filesystem;

class WebSocketSessionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const std::string start_cmd = "start_websocat_echo start";
        std::system(start_cmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "websocat server should be ready now" << std::endl;
    }

    void TearDown() override
    {
        const std::string stop_cmd = "start_websocat_echo stop";
        std::system(stop_cmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::system("pkill -f websocat || true");
    }
};

TEST_F(WebSocketSessionTest, EchoRoundTrip)
{
    asio::io_context ioc;
    ssl::context     ssl_ctxt(ssl::context::tls_client);
    ssl_ctxt.set_verify_mode(ssl::context::verify_none);

    std::promise<std::string> got;
    auto                      fut = got.get_future();

    const WebSocketSessionConfig config{
        .host = "localhost", .port = "9001", .endpoint = "/", .auth_msg = "", .sub_msg = "", .ssl_ctxt = ssl_ctxt};

    auto wss =
        WebSocketSession::create(ioc, config, [&](const std::string_view frame) { got.set_value(std::string(frame)); });

    wss->start();
    asio::post(ioc, [wss] { wss->send("ping"); });
    std::thread t([&] { ioc.run(); });

    const auto wait_result = fut.wait_for(std::chrono::seconds(10));
    if (wait_result != std::future_status::ready)
    {
        std::cout << "Test timed out waiting for WebSocket response" << std::endl;
        std::cout << "Check if websocat server is running properly" << std::endl;
    }
    ASSERT_EQ(wait_result, std::future_status::ready);
    EXPECT_EQ(fut.get(), "ping");

    wss->stop();
    ioc.stop();
    t.join();
}
