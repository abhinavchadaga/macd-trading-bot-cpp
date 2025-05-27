#pragma once

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <openssl/ssl.h>
#include <queue>
#include <string>
#include <string_view>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

struct web_socket_session_config
{
  std::string host;
  std::string port;
  std::string endpoint;
  std::string auth_msg;
  std::string sub_msg;
  ssl::context &ssl_ctxt;
};

class web_socket_session
    : public std::enable_shared_from_this<web_socket_session>
{
public:
  using frame_handler = std::function<void (std::string_view)>;

  static std::shared_ptr<web_socket_session>
  create (net::io_context &ioc, const web_socket_session_config &config,
          frame_handler on_frame);

  explicit web_socket_session (net::io_context &ioc,
                               web_socket_session_config cfg);

  void start ();

  void stop ();

  void send (const std::string_view text);

private:
  void fail (const beast::error_code &ec, char const *what);

  void resolve ();

  void on_resolve (const beast::error_code &ec,
                   const tcp::resolver::results_type &results);

  void
  on_connect (beast::error_code error_code,
              const tcp::resolver::results_type::endpoint_type &endpoint_type);

  void on_ssl_handshake (const beast::error_code &error_code);

  void on_handshake (const beast::error_code &ec);

  void do_read ();

  void on_read (const beast::error_code &ec, std::size_t bytes_transferred);

  void do_write ();

  void on_write (const beast::error_code &ec, std::size_t bytes_transferred);

  void arm_heartbeat ();

  void on_ping_timer (const beast::error_code &ec);

  void reconnect ();

  web_socket_session_config _config;

  //
  // Boost::Beast state
  tcp::resolver _resolver;
  websocket::stream<beast::ssl_stream<beast::tcp_stream> > _ws;
  beast::flat_buffer _buffer;
  net::steady_timer _ping_timer;

  std::string _host_port;

  frame_handler _frame_handler;
  std::queue<std::string> _write_queue;

  bool _connected;
  bool _should_reconnect;
};
