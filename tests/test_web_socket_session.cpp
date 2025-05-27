#include "web_socket_session.hpp"
#include <boost/asio.hpp>
#include <gtest/gtest.h>
#include <future>
#include <thread>
#include <chrono>

namespace asio = boost::asio;
namespace ssl = asio::ssl;

TEST (WebSocketSession, EchoRoundTrip)
{
  asio::io_context ioc;
  ssl::context ssl_ctxt (ssl::context::tls_client);
  ssl_ctxt.set_verify_mode (ssl::context::verify_none);

  std::promise<std::string> got;
  auto fut = got.get_future ();

  const web_socket_session_config config{
    .host = "127.0.0.1",
    .port = "9001",
    .endpoint = "/",
    .auth_msg = "",
    .sub_msg = "",
    .ssl_ctxt = ssl_ctxt
  };

  auto wss = web_socket_session::create (
      ioc, config,
      [&](const std::string_view frame)
      {
        got.set_value (std::string (frame));
      });

  wss->start ();
  asio::post (ioc, [wss] { wss->send ("ping"); });
  std::thread t ([&] { ioc.run (); });

  ASSERT_EQ (fut.wait_for(std::chrono::seconds(2)),
             std::future_status::ready);
  EXPECT_EQ (fut.get(), "ping");

  wss->stop ();
  ioc.stop ();
  t.join ();
}